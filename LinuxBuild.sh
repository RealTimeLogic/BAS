#!/bin/bash
# Compile Mako Server as follows:
#  wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh | bash
# In Bash, download Developer Edition resources without requiring the zip tool:
#  MAKO_ZIP_MODE=developer bash <(wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh)
# MAKO_ZIP_MODE is an optional string: build (default) or developer.
# XLIB is an optional linker-library string; when unset, Linux libraries are probed.
# CC is an optional compiler command; defaults to ${CROSS_COMPILE}gcc or gcc.
# Details: https://github.com/RealTimeLogic/BAS


function abort() {
    printf "%b\n\n" "$1";
    read -p "Press ENTER to exit script"
    exit 1;
}

function install() {
    local missing=("$@")
    local apt_cmd

    printf "Missing required commands: %s\n" "${missing[*]}"

    if ! command -v apt-get >/dev/null 2>&1; then
        abort "Automatic installation requires apt-get. Install the missing commands manually: ${missing[*]}"
    fi

    [ -r /dev/tty ] || abort "Cannot prompt for package installation. Run manually:\nsudo apt-get install ${missing[*]}"
    read -p "Do you want to install the missing tools with apt-get (Y/n)?" </dev/tty || abort "Could not read package installation response"
    if [ "$REPLY" = "n" ] || [ "$REPLY" = "N" ]; then
        abort "Run the following prior to running this script:\nsudo apt-get install ${missing[*]}"
    fi

    if [ "$(id -u)" -eq 0 ]; then
        apt_cmd=(apt-get)
    else
        command -v sudo >/dev/null 2>&1 || abort "Missing sudo. Run as root or install manually:\napt-get install ${missing[*]}"
        apt_cmd=(sudo apt-get)
    fi

    "${apt_cmd[@]}" update || abort "apt-get update failed"
    "${apt_cmd[@]}" install -y "${missing[@]}" || abort "Installing required commands failed"
}

function clone_or_update() {
    local dir="$1"
    local url="$2"

    if [ -d "$dir/.git" ]; then
        printf "Updating %s\n" "$dir"
        git -C "$dir" pull --ff-only || abort "Updating $dir failed"
    elif [ -e "$dir" ]; then
        abort "'$dir' already exists but is not a Git repository"
    else
        printf "Cloning %s\n" "$dir"
        git clone "$url" "$dir" || abort "Cloning $dir failed"
    fi
}

function download_sqlite_amalgamation() {
    local html product_line product version relative_url size sha url archive tmpdir extracted_dir

    if [ -n "${SQLITEURL+set}" ]; then
        url="$SQLITEURL"
    else
        printf "Fetching latest SQLite amalgamation metadata\n"
        html="$(curl -fsSL https://sqlite.org/download.html)" || abort "Could not fetch SQLite download metadata"
        product_line="$(
            printf "%s\n" "$html" |
                grep -E "PRODUCT,[0-9.]+,[0-9]{4}/sqlite-amalgamation-[0-9]+\.zip," |
                head -n 1
        )"
        [ -n "$product_line" ] || abort "Could not determine latest SQLite amalgamation URL"

        IFS=',' read -r product version relative_url size sha <<<"$product_line"
        url="https://sqlite.org/${relative_url}"
    fi

    archive="${url##*/}"
    tmpdir="$(mktemp -d)" || abort "Could not create temporary directory"

    printf "Downloading SQLite amalgamation: %s\n" "$url"
    curl -fL "$url" -o "${tmpdir}/${archive}" || abort "Downloading SQLite amalgamation failed"

    unzip -q -o "${tmpdir}/${archive}" -d "$tmpdir" || abort "Unpacking SQLite amalgamation failed"
    extracted_dir="$(
        find "$tmpdir" -mindepth 1 -maxdepth 1 -type d -name "sqlite-amalgamation-*" |
            head -n 1
    )"
    [ -n "$extracted_dir" ] || abort "Downloaded SQLite archive did not contain sqlite-amalgamation-*"

    mkdir -p src
    cp "${extracted_dir}"/* src/ || abort "Installing SQLite amalgamation failed"
    rm -rf "$tmpdir"

    printf "Placed SQLite amalgamation in BAS/src\n"
}

function select_linux_libraries() {
    local tmpdir candidate
    local compile_flags link_flags libraries

    # Preserve explicit overrides, including an intentionally empty XLIB.
    [ -n "${XLIB+set}" ] && return
    tmpdir="$(mktemp -d)" || abort "Could not create linker probe directory"
    cat > "$tmpdir/probe.c" <<'EOF'
#include <pthread.h>
#include <dlfcn.h>
#include <math.h>
static void *worker(void *p)
{
   return p;
}
int main(int argc, char **argv)
{
   pthread_t thread;
   volatile double value = argc;
   void *handle = dlopen(0, RTLD_NOW);
   void *symbol = dlsym(handle, "main");
   int status = pthread_create(&thread, 0, worker, argv);
   if (!status)
   {
      status = pthread_join(thread, 0);
   }
   if (handle)
   {
      dlclose(handle);
   }
   return status + (int)cos(value) + (symbol != 0);
}
EOF
    read -r -a compile_flags <<< "${CFLAGS:-}"
    read -r -a link_flags <<< "${HEXEFLAGS:-}"
    # Link only: the selected compiler may target another machine.
    for candidate in '-lpthread -lm -ldl' '-lm'; do
        read -r -a libraries <<< "$candidate"
        printf 'Trying XLIB=%s\n' "$candidate" >> "$tmpdir/link.log"
        if "${compiler[@]}" "${compile_flags[@]}" "${link_flags[@]}" \
            "$tmpdir/probe.c" -o "$tmpdir/probe" "${libraries[@]}" \
            >> "$tmpdir/link.log" 2>&1; then
            XLIB="$candidate"
            rm -rf "$tmpdir"
            return
        fi
    done
    cat "$tmpdir/link.log" >&2
    rm -rf "$tmpdir"
    abort "Could not link with the selected compiler. Check the toolchain or set XLIB explicitly."
}

REPOS=(
    "BAS|https://github.com/RealTimeLogic/BAS.git"
    "BAS-Resources|https://github.com/RealTimeLogic/BAS-Resources.git"
    "LPeg|https://github.com/roberto-ieru/LPeg.git"
    "lua-protobuf|https://github.com/starwing/lua-protobuf.git"
    "CBOR|https://github.com/spc476/CBOR.git"
)

DEVELOPER_MAKO_ZIP_URL="https://makoserver.net/download/packages/mako.zip"
DEVELOPER_MAKO_ZIP_INFO_URL="https://makoserver.net/documentation/developer-package/"

function copy_file() {
    local src="$1"
    local dst="$2"

    if cp "$src" "$dst" 2>/dev/null; then
        return 0
    fi

    command -v sudo >/dev/null 2>&1 || abort "Could not write $dst. Run as root or install sudo."
    sudo cp "$src" "$dst" || abort "Could not write $dst"
}

function open_developer_mako_zip_info() {
    if command -v xdg-open >/dev/null 2>&1; then
        xdg-open "$DEVELOPER_MAKO_ZIP_INFO_URL" >/dev/null 2>&1 &
    elif command -v open >/dev/null 2>&1; then
        open "$DEVELOPER_MAKO_ZIP_INFO_URL" >/dev/null 2>&1 &
    elif command -v cygstart >/dev/null 2>&1; then
        cygstart "$DEVELOPER_MAKO_ZIP_INFO_URL" >/dev/null 2>&1 &
    else
        printf "Open this page for more information: %s\n" "$DEVELOPER_MAKO_ZIP_INFO_URL"
    fi
}

function validate_mako_zip() {
    local archive="$1" entry contents

    # -p also works with BusyBox unzip, which may not provide Info-ZIP's -t.
    unzip -p "$archive" > /dev/null || return 1
    for entry in .config .openports; do
        contents="$(unzip -p "$archive" "$entry")" || return 1
        [ -n "$contents" ] || return 1
    done
}

function download_developer_mako_zip() {
    local target_zip="$1" tmpdir tmpzip backup

    # Stage beside the destination so the final rename stays on one filesystem.
    tmpdir="$(mktemp -d "${target_zip}.download.XXXXXX")" || abort "Could not create download directory"
    tmpzip="${tmpdir}/mako.zip"
    printf "Downloading Mako Developer Edition mako.zip\n"
    if ! curl -fL "$DEVELOPER_MAKO_ZIP_URL" -o "$tmpzip" || \
        ! validate_mako_zip "$tmpzip"; then
        rm -rf "$tmpdir"
        abort "Downloading or validating Mako Developer Edition mako.zip failed"
    fi
    if [ -e "$target_zip" ]; then
        backup="${target_zip}.bak-$(date +%Y%m%d-%H%M%S)-$$"
        if ! cp "$target_zip" "$backup"; then
            rm -rf "$tmpdir"
            abort "Could not back up $target_zip"
        fi
        printf "Backed up %s to %s\n" "$target_zip" "$backup"
    fi
    if ! mv -f "$tmpzip" "$target_zip"; then
        rm -rf "$tmpdir"
        abort "Could not replace $target_zip"
    fi
    rmdir "$tmpdir"
}

function ask_install_developer_mako_zip() {
    local target_zip="$1"

    [ -f "$target_zip" ] || abort "Cannot find mako.zip to replace: $target_zip"

    printf "\nMako Developer Edition mako.zip adds FuguHub, Xedge, LSP-Claw, and local MQTT development tools.\n"
    printf "If you install it, this script backs up the current mako.zip before replacing it.\n"

    if [ -r /dev/tty ]; then
        read -p "Do you want more information in your browser (y/N)?" </dev/tty || abort "Could not read response"
        if [ "$REPLY" = "y" ] || [ "$REPLY" = "Y" ]; then
            open_developer_mako_zip_info
        fi
    fi

    [ -r /dev/tty ] || abort "Cannot prompt for Mako Developer Edition installation"
    read -p "Do you want to install Mako Developer Edition mako.zip (y/N)?" </dev/tty || abort "Could not read response"
    if [ "$REPLY" != "y" ] && [ "$REPLY" != "Y" ]; then
        return 0
    fi

    download_developer_mako_zip "$target_zip"
}

MAKO_ZIP_MODE="${MAKO_ZIP_MODE:-build}"
case "$MAKO_ZIP_MODE" in
    build|developer) ;;
    *) abort "Invalid MAKO_ZIP_MODE '$MAKO_ZIP_MODE'; use build or developer." ;;
esac

CC="${CC:-${CROSS_COMPILE}gcc}"
read -r -a compiler <<< "$CC"
executables=(git curl unzip make "${compiler[0]}")
[ "$MAKO_ZIP_MODE" = build ] && executables+=(zip)
missing=()

for i in "${executables[@]}"; do
    if ! command -v "$i" &> /dev/null; then
        missing+=("$i")
    fi
done

if ((${#missing[@]})); then
    install "${missing[@]}"
    for i in "${missing[@]}"; do
        command -v "$i" >/dev/null 2>&1 || abort "Required command is still missing after install: $i"
    done
fi


unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     XCFLAGS=-DLUA_USE_LINUX;machine=Linux;export BA_USE_EPOLL=1;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    XCFLAGS="-DLUA_USE_LINUX -DBA_ENABLE_LUA_FORKPTY=0";machine=Cygwin;;
#    MINGW*)     machine=MinGw;;
    *)          abort "Unknown machine ${unameOut}"
esac

echo "Using compiler $CC"

if [ -f src/BAS.c ]; then
    abort "Incorrect use! This script should not be run in the BAS directory.\nDetails: https://github.com/RealTimeLogic/BAS"
fi

for repo in "${REPOS[@]}"; do
    IFS='|' read -r dir url <<<"$repo"
    clone_or_update "$dir" "$url"
done

if [ -d "BAS-Resources/build" ]; then
    shopt -s nullglob
    build_scripts=(BAS-Resources/build/*.sh)
    if ((${#build_scripts[@]})); then
        chmod +x "${build_scripts[@]}"
    fi
    shopt -u nullglob
fi


cd BAS || abort $LINENO
if ! [ -f "src/sqlite3.c" ]; then
    download_sqlite_amalgamation
fi

if [ -n "${NOCOMPILE+set}" ]; then
    exit 0
fi

make_args=("CC=$CC")
[ "$machine" = Linux ] && select_linux_libraries
if [ -n "${XLIB+set}" ]; then
    printf 'Using link libraries: %s\n' "$XLIB"
    make_args+=("XLIB=$XLIB")
fi

if [ "$MAKO_ZIP_MODE" = developer ]; then
    download_developer_mako_zip mako.zip
else
    # Regenerate after source updates or a previous Developer Edition build.
    make -f mako.mk "${make_args[@]}" -B mako.zip || abort $LINENO
    validate_mako_zip mako.zip || abort "Generated mako.zip is invalid"
fi
make -f mako.mk "${make_args[@]}" || abort $LINENO

if [ "$MAKO_ZIP_MODE" = build ]; then
    ask_install_developer_mako_zip mako.zip
fi

if [[ -z "${CROSS_COMPILE}" ]]; then
    read -p "Do you want to install the Mako Server in /usr/local/bin (Y/n)?" </dev/tty
    if [ "$REPLY" != "n" ]; then
        mkdir -p /usr/local/bin 2>/dev/null || \
            sudo mkdir -p /usr/local/bin || abort "Could not create /usr/local/bin"
        copy_file mako /usr/local/bin/mako
        copy_file mako.zip /usr/local/bin/mako.zip
        echo "Installed; you may now run mako"
        MAKO=mako
    else
        MAKO=BAS/mako
    fi
else
    echo "Done"
    echo "You may now run BAS/mako"
    MAKO=BAS/mako
fi

cd ..
read -p "Do you want to download and run the Lua tutorials (Y/n)?"
if [ "$REPLY" != "n" ]; then
    ${MAKO} -l::BAS-Resources/tools/MakoDownloadTutorials
fi

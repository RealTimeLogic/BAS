#!/bin/bash
# Compile Mako Server as follows:
#  wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh | bash
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

function ask_install_developer_mako_zip() {
    local target_zip="$1"
    local tmpdir tmpzip backup

    [ -f "$target_zip" ] || abort "Cannot find mako.zip to replace: $target_zip"

    printf "\nMako Developer Edition mako.zip adds Xedge, LSP-Claw, and local MQTT development tools.\n"
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

    tmpdir="$(mktemp -d)" || abort "Could not create temporary directory"
    tmpzip="${tmpdir}/mako.zip"
    backup="${target_zip}.bak-$(date +%Y%m%d-%H%M%S)"

    printf "Downloading Mako Developer Edition mako.zip\n"
    curl -fL "$DEVELOPER_MAKO_ZIP_URL" -o "$tmpzip" || abort "Downloading Mako Developer Edition mako.zip failed"

    printf "Backing up %s to %s\n" "$target_zip" "$backup"
    copy_file "$target_zip" "$backup"

    printf "Installing Mako Developer Edition mako.zip to %s\n" "$target_zip"
    copy_file "$tmpzip" "$target_zip"
    rm -rf "$tmpdir"
}

executables="git curl zip unzip gcc make"
missing=()

for i in $executables; do
    if ! command -v $i &> /dev/null; then
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
    Linux*)     XLIB=-ldl;XCFLAGS=-DLUA_USE_LINUX;machine=Linux;export EPOLL=TRUE;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    XLIB=-ldl;XCFLAGS="-DLUA_USE_LINUX -DUSE_FORKPTY=0";machine=Cygwin;;
#    MINGW*)     machine=MinGw;;
    *)          abort "Unknown machine ${unameOut}"
esac

if [  -z ${CC+x} ]; then
    command -v gcc >/dev/null 2>&1 || install gcc
    CC=gcc
    echo "Setting default compiler"
fi
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

make -f mako.mk || abort $LINENO

MAKO_ZIP=mako.zip
if [[ -z "${CROSS_COMPILE}" ]]; then
    read -p "Do you want to install the Mako Server in /usr/local/bin (Y/n)?" </dev/tty
    if [ "$REPLY" != "n" ]; then
        sudo cp mako mako.zip /usr/local/bin/ || abort
        echo "Installed; you may now run mako"
        MAKO_ZIP=/usr/local/bin/mako.zip
        MAKO=mako
    else
        MAKO=BAS/mako
    fi
else
    echo "Done"
    echo "You may now run BAS/mako"
    MAKO=BAS/mako
fi

ask_install_developer_mako_zip "$MAKO_ZIP"

cd ..
read -p "Do you want to download and run the Lua tutorials (Y/n)?"
if [ "$REPLY" != "n" ]; then
    ${MAKO} -l::BAS-Resources/tools/MakoDownloadTutorials
fi

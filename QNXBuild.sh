#!/bin/bash
# Compile Mako Server for QNX as follows:
#  wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/QNXBuild.sh | bash


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

if [ -z "${QNX_TARGET+x}" ]; then
    abort "Oops, you must source qnxsdp-env.sh before running this script."
fi

executables="git curl zip unzip make"
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

command -v qcc >/dev/null 2>&1 || abort "Cannot find qcc. Make sure qnxsdp-env.sh is sourced correctly."

if [ -n "$TARGET" ]; then
    export HEXEFLAGS="-Vgcc_nto$TARGET"
else
    export HEXEFLAGS="-Vgcc_ntox86_64"
fi
echo "Target compile flag: $HEXEFLAGS"

export CFLAGS="$HEXEFLAGS -DBA_QNX -DBA_64BIT -DBA_FILESIZE64 -DUSE_DBGMON=1 -DNDEBUG -DLUA_USE_LINUX -DMAKO -DUSE_EMBEDDED_ZIP=0 -DUSE_IPV6 -DNO_IPV6_MEMBERSHIP -D_POSIX"
export CC=qcc
export XLIB="-lz -lsocket -lm"

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

echo "Done building 'mako' for QNX"

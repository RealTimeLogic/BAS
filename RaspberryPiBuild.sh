#!/bin/bash
# Compile Mako Server and embedded Linux Lua bindings as follows:
# wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/RaspberryPiBuild.sh | bash
# Details: https://makoserver.net/download/raspberry-pi/
# This script automates: https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design

function abort() {
    printf "%b\n" "$1"
    echo "See the following tutorial for how to manually build the code:"
    echo "https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design"
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

function clone_or_update_recursive() {
    local dir="$1"
    local url="$2"

    if [ -d "$dir/.git" ]; then
        if git -C "$dir" diff --quiet && git -C "$dir" diff --cached --quiet; then
            printf "Updating %s\n" "$dir"
            git -C "$dir" pull --ff-only || abort "Updating $dir failed"
            git -C "$dir" submodule update --init --recursive || abort "Updating $dir submodules failed"
        else
            printf "Skipping update for %s; working tree has local changes\n" "$dir"
            git -C "$dir" submodule update --init --recursive || abort "Updating $dir submodules failed"
        fi
    elif [ -e "$dir" ]; then
        abort "'$dir' already exists but is not a Git repository"
    else
        printf "Cloning %s\n" "$dir"
        git clone --recursive "$url" "$dir" || abort "Cloning $dir failed"
    fi
}

function download_zip_resource() {
    local dir="$1"
    local url="$2"
    local archive tmpdir

    if [ -d "$dir" ]; then
        printf "Using existing %s\n" "$dir"
        return 0
    elif [ -e "$dir" ]; then
        abort "'$dir' already exists but is not a directory"
    fi

    archive="${url##*/}"
    tmpdir="$(mktemp -d)" || abort "Could not create temporary directory"

    printf "Downloading %s\n" "$archive"
    curl -fL "$url" -o "${tmpdir}/${archive}" || abort "Cannot download $archive"
    unzip -q "${tmpdir}/${archive}" -d . || abort "Unzip failed"
    rm -rf "$tmpdir"

    [ -d "$dir" ] || abort "Downloaded archive did not create $dir"
}

function patch_lua_periphery() {
    local src="lua-periphery/src/lua_periphery.c"
    local makefile="lua-periphery/Makefile"

    [ -f "$src" ] || abort "Cannot find $src"
    [ -f "$makefile" ] || abort "Cannot find $makefile"

    if ! grep -q "include <luaintf.h>" "$src"; then
        sed -i -e 's/include <lua.h>/include <luaintf.h>/' "$src" || abort "Patching $src failed"
    fi

    if ! grep -q "luaintf(L);" "$src"; then
        sed -i -re 's%(luaopen_periphery[^{]+[{])%\1\nluaintf(L);\n%' "$src" || abort "Patching $src failed"
    fi

    if ! grep -q "../MakoModuleExample/src/lua/luaintf.c" "$makefile"; then
        sed -i -e 's:SRCS =*:&../MakoModuleExample/src/lua/luaintf.c:' "$makefile" || abort "Patching $makefile failed"
    fi
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


export NOCOMPILE=true
curl -fsSL https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh | bash || abort "LinuxBuild.sh failed"

cd BAS || abort

echo "Compiling Mako Server"
export EPOLL=true
make -f mako.mk || abort
cd ..

download_zip_resource "MakoModuleExample" "https://makoserver.net/download/MakoModuleExample.zip"

clone_or_update_recursive "lua-periphery" "https://github.com/vsergeev/lua-periphery.git"
patch_lua_periphery

echo "Building the Raspberry Pi GPIO Lua Bindings (lua-periphery)"
cd lua-periphery || abort
export LUA_INCDIR=../MakoModuleExample/src/lua
make || abort
cd ..

if [[ -z "${CROSS_COMPILE}" ]]; then
    read -p "Do you want to install the Mako Server in /usr/local/bin (Y/n)?" </dev/tty
    if [ "$REPLY" != "n" ]; then
        sudo cp BAS/mako BAS/mako.zip lua-periphery/periphery.so /usr/local/bin/ || abort
        echo "Installed"
        exit 0
    fi
fi
cp lua-periphery/periphery.so BAS/
echo "The produced files mako, mako.zip, and periphery.so can be found in the BAS/ directory"

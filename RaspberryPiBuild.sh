#!/bin/bash
# Compile Mako Server and embedded Linux Lua bindings as follows:
# wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/RaspberryPiBuild.sh | bash
# Details: https://makoserver.net/download/raspberry-pi/
# This script automates: https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design

function abort() {
    echo "See the following tutorial for how to manually build the code:"
    echo "https://makoserver.net/articles/Expedite-Your-Embedded-Linux-Web-Interface-Design"
    read -p "Press ENTER to exit script"
    exit 1;
}

export NOCOMPILE=true
wget --no-check-certificate -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh | bash || abort"LinuxBuild.sh failed"
cd BAS/src || abort

if ! [ -f "lpeg/lpcode.c" ]; then
    echo "Downloading Lua lpeg"
    wget http://www.inf.puc-rio.br/~roberto/lpeg/lpeg-1.0.2.tar.gz
    tar xzf lpeg-1.0.2.tar.gz
    rm lpeg-1.0.2.tar.gz
    mv lpeg-1.0.2 lpeg || abort
fi

if ! [ -f "lua-protobuf/pb.c" ]; then
    echo "Downloading Google Protobuf for Lua"
    git clone https://github.com/starwing/lua-protobuf
fi

cd ..

echo "Compiling Mako Server"
export EPOLL=true
make -f mako.mk || abort
cd ..

if ! [ -f "MakoModuleExample/README.txt" ]; then
    echo "Downloading the Mako Server's Lua C Code Module Library"
    wget --no-check-certificate https://makoserver.net/download/MakoModuleExample.zip || abort"Cannot download MakoModuleExample.zip"
    unzip MakoModuleExample.zip || abort"Unzip failed"
    rm  MakoModuleExample.zip
fi

if ! [ -f "lua-periphery/README.md" ]; then
    echo "Cloning and building the Raspberry Pi GPIO Lua Bindings (lua-periphery)"
    git clone --recursive https://github.com/vsergeev/lua-periphery.git || abort
    cd lua-periphery
    sed -i -e 's/include <lua.h>/include <luaintf.h>/' src/lua_periphery.c || abort
    sed -i -re 's%(luaopen_periphery[^{]+[{])%\1\nluaintf(L);\n%' src/lua_periphery.c || abort
    sed -i -e 's:SRCS =*:&../MakoModuleExample/src/lua/luaintf.c:' Makefile || abort
    export LUA_INCDIR=../MakoModuleExample/src/lua || abort
    make || abort
    cd ..
fi

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


#!/bin/bash
# Compile Mako Server as follows:
#  wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/LinuxBuild.sh | bash
# Details: https://github.com/RealTimeLogic/BAS


function abort() {
    printf "$1\n\n";
    read -p "Press ENTER to exit script"
    exit 1;
}

function install() {
    abort "Run the following prior to running this script:\nsudo apt-get install git zip unzip gcc make"
}

executables="git zip unzip gcc make"

for i in $executables; do
    if ! command -v $i &> /dev/null; then
        install
        exit 1
    fi
done


unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     XLIB=-ldl;XCFLAGS=-DLUA_USE_LINUX;machine=Linux;export EPOLL=TRUE;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    XLIB=-ldl;XCFLAGS="-DLUA_USE_LINUX -DUSE_FORKPTY=0";machine=Cygwin;;
#    MINGW*)     machine=MinGw;;
    *)          abort "Unknown machine ${unameOut}"
esac

if [  -z ${CC+x} ]; then
    command -v gcc >/dev/null 2>&1 || install
    CC=gcc
    echo "Setting default compiler"
fi
echo "Using compiler $CC"

if [ -f src/BAS.c ]; then
    abort "Incorrect use! This script should not be run in the BAS directory.\nDetails: https://github.com/RealTimeLogic/BAS"
fi

if ! [ -d "BAS" ]; then
    git clone https://github.com/RealTimeLogic/BAS.git || abort "Cloning BAS failed"
fi

if ! [ -d "BAS-Resources" ]; then
    git clone https://github.com/RealTimeLogic/BAS-Resources.git || abort "Cloning BAS-Resources failed"
    chmod +x BAS-Resources/build/*.sh
fi

if ! [ -f "LPeg/lpcode.c" ]; then
    echo "Downloading LPeg"
    git clone https://github.com/roberto-ieru/LPeg.git LPeg
fi

if ! [ -f "lua-protobuf/pb.c" ]; then
    echo "Downloading Google Protobuf for Lua"
    git clone https://github.com/starwing/lua-protobuf.git lua-protobuf
fi

if ! [ -f "CBOR/cbor_c.c" ]; then
    echo "Downloading CBOR"
    git clone https://github.com/spc476/CBOR.git CBOR
fi



cd BAS || abort $LINENO
if ! [ -f "src/sqlite3.c" ]; then
    # if SQLITEURL url not set
    if [  -z ${SQLITEURL+x} ]; then
        # There is no 'latest' with SQLite :-(
	SQLITEURL="https://www.sqlite.org/2025/sqlite-amalgamation-3490100.zip"
    fi
    SQLITE=${SQLITEURL##*/}
    pushd /tmp || abort $LINENO
    echo "Downloading: $SQLITEURL"
    command -v wget >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        wget --no-check-certificate $SQLITEURL || abort $LINENO
    else
        curl $SQLITEURL -o $SQLITE
    fi
    unzip -o $SQLITE || abort $LINENO
    popd
    mv /tmp/${SQLITE%.zip}/* src/ || abort $LINENO
fi

pushd ../BAS-Resources/build || abort $LINENO
echo "Building mako.zip"
./mako.sh || abort $LINENO
cp mako.zip ../../ || abort $LINENO
popd

if [ -n "${NOCOMPILE+set}" ]; then
    exit 0
fi

make -f mako.mk || abort $LINENO
cp examples/MakoServer/mako* || abort $LINENO
cd ..

if [[ -z "${CROSS_COMPILE}" ]]; then
    read -p "Do you want to install the Mako Server in /usr/local/bin (Y/n)?" </dev/tty
    if [ "$REPLY" != "n" ]; then
        sudo cp BAS/mako BAS/mako.zip /usr/local/bin/ || abort
        echo "Installed; you may now run mako"
        exit 0
    fi
fi

echo "Done"
echo "You may now run BAS/mako"

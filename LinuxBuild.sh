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
    Linux*)     XLIB=-ldl;XCFLAGS=-DLUA_USE_LINUX;machine=Linux;;
    Darwin*)    XLIB="-ldl -framework IOKit -framework CoreFoundation;XCFLAGS=-DLUA_USE_LINUX;machine=Linux";XCFLAGS="-D_OSX_ -DLUA_USE_MACOSX";machine=Mac;;
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


cd BAS || abort $LINENO
if ! [ -f "src/sqlite3.c" ]; then
    if [  -z ${SQLITEURL+x} ]; then
	SQLITEURL="https://www.sqlite.org/2022/sqlite-amalgamation-3400000.zip"
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

echo "Compiling using $CC; this may take some time........"
$CC -o examples/MakoServer/mako -fmerge-all-constants -O3 -Os -w\
    $XCFLAGS -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DMAKO\
    -DUSE_LUAINTF\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    src/ls_sqlite3.c src/luasql.c src/sqlite3.c\
    -lpthread -lm $XLIB || abort $LINENO

cp examples/MakoServer/mako* ../ || abort $LINENO
echo "Done"
echo "You may now run ./mako"

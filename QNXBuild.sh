#!/bin/bash
# Compile Mako Server for QNX as follows:
#  wget -O - https://raw.githubusercontent.com/RealTimeLogic/BAS/main/QNXBuild.sh | bash


function abort() {
    printf "$1\n\n";
    read -p "Press ENTER to exit script"
    exit 1;
}

if [ -z "${QNX_TARGET+x}" ]; then
    abort "Oops, you must source qnxsdp-env.sh before running this script."
fi

executables="git zip unzip make"
for i in $executables; do
    if ! command -v $i &> /dev/null; then
        abort "Run the following prior to running this script:\nsudo apt-get install git zip unzip make"
        exit 1
    fi
done

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

if [ -n "${NOCOMPILE+set}" ]; then
    exit 0
fi

make -f mako.mk || abort $LINENO

echo "Done building 'mako' for QNX"

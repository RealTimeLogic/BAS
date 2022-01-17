
function abort() {
    printf "$1\n\n";
    read -p "Press ENTER to exit script"
    exit 1;
}

function install() {
    abort "Run the following prior to running this script:\nsudo apt-get install git unzip gcc"
}

if [  -z ${CC+x} ]; then
    command -v gcc >/dev/null 2>&1 || install
    echo "Using the gcc compiler"
else
    echo "Using compiler $CC"
fi
command -v git >/dev/null 2>&1 || install
command -v unzip >/dev/null 2>&1 || install

if [ -f src/BAS.c ]; then
    abort "Incorrect use! This script should not be run in the BAS directory.\nDetails: https://github.com/RealTimeLogic/BAS"
fi

if ! [ -d "BAS" ]; then
    git clone https://github.com/RealTimeLogic/BAS.git || abort "Cloning BAS failed"
fi

cd BAS || abort $LINENO
if ! [ -f "src/sqlite3.c" ]; then
    if [  -z ${SQLITE+x} ]; then
        SQLITE="https://www.sqlite.org/2022/sqlite-amalgamation-3370200.zip"
    fi
    pushd /tmp || abort $LINENO
    echo "Downloading: $SQLITE"
    wget --no-check-certificate $SQLITE || abort $LINENO
    SQLITE=${SQLITE##*/}
    unzip $SQLITE || abort $LINENO
    popd
    mv /tmp/${SQLITE%.zip}/* src/ || abort $LINENO
fi

echo "Compiling using $CC........"
$CC -o examples/MakoServer/mako -fmerge-all-constants -O3 -Os -w\
    -DUSE_EMBEDDED_ZIP=0 -DBA_FILESIZE64 -DBA_64BIT -DLUA_USE_LINUX -DMAKO\
    -Iinc -Iinc/arch/Posix -Iinc/arch/NET/Posix\
    src/BAS.c\
    src/arch/Posix/ThreadLib.c src/arch/NET/generic/SoDisp.c src/DiskIo/posix/BaFile.c\
    examples/MakoServer/src/MakoMain.c\
    src/ls_sqlite3.c src/luasql.c src/sqlite3.c\
    -lpthread -lm -ldl || abort $LINENO

cp examples/MakoServer/mako* ../ || abort $LINENO
echo "Done"
echo "You may now run ./mako"


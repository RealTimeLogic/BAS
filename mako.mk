# Makefile for compiling the Mako Server for Linux, Mac, QNX, Windows
# with GNU Make Example:
# make -f mako.mk EPOLL=true
# The above compiles mako using the 'epoll' socket dispatcher for Linux. The
# default is to use the 'select' socket dispatcher.
# The makefile is designed for the "Embedded Linux Web Based Device
# Management" tutorial and will auto include the generated Lua
# bindings if found. The makefile will also auto include SQLite, Lua
# Protobuf, CBOR, and LPeg if found. LPeg can be installed as follows:
# wget http://www.inf.puc-rio.br/~roberto/lpeg/lpeg-1.0.2.tar.gz
# cd src
# tar xvzf ../lpeg-1.0.2.tar.gz
# mv lpeg-1.0.2 lpeg

ifdef CROSS_COMPILE
CC = $(CROSS_COMPILE)gcc
endif

ifeq ($(OS),Windows_NT)
WINDOWS=1
ifeq ($(origin CC),default)
CC = cl
endif
TARGET = mako.exe
OBJEXT = obj
else
TARGET = mako
OBJEXT = o
endif

#Required
ifeq (,$(wildcard ../BAS-Resources/build))
$(error ../BAS-Resources not found. Repository https://github.com/RealTimeLogic/BAS-Resources required!)
endif

USE_OPCUA?=1
DEBUG?=0


#Optional
export LPEGDIR=../LPeg
export PROTOBUFDIR=../lua-protobuf
export CBORDIR=../CBOR

ifeq ($(WINDOWS),1)

XLIB=ws2_32.lib advapi32.lib user32.lib shell32.lib
D=/D
I=/I
DEBUG_CFLAGS=/Od /Zi $(D)_DEBUG
RELEASE_CFLAGS=/O2 $(D)NDEBUG
CFLAGS += /nologo /W3

# Add required macros and Windows configuration.
CFLAGS += $(D)B_LITTLE_ENDIAN $(D)_CRT_SECURE_NO_WARNINGS $(D)WIN32_LEAN_AND_MEAN=1 $(D)WIN32 $(D)_CONSOLE

CFLAGS += $(I)inc $(I)inc/arch/Windows $(I)inc/arch/NET/Windows

VPATH = src src/arch/Windows src/arch/NET/generic src/DiskIo/windows examples/MakoServer/src examples/MakoServer/src/Windows

SOURCE = BAS.c ThreadLib.c SoDisp.c BaFile.c MakoMain.c MakoWinMain.c servutil.c

else

D=-D
I=-I
DEBUG_CFLAGS=-g -O0
RELEASE_CFLAGS=-O3 -Os $(D)NDEBUG
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	XLIB= -lpthread -lm -ldl -framework IOKit -framework CoreFoundation;
	CFLAGS += $(D)_OSX_ $(D)LUA_USE_MACOSX
endif

ifndef XLIB
XLIB= -lpthread -lm -ldl
endif

CFLAGS += -fmerge-all-constants -Wall

# Add required macros and enable large-file support
CFLAGS += $(D)LUA_USE_LINUX $(D)BA_FILESIZE64

CFLAGS += $(I)inc $(I)inc/arch/Posix
ifdef EPOLL
SODISP = epoll
CFLAGS += $(I)inc/arch/NET/epoll
else
SODISP = generic
CFLAGS += $(I)inc/arch/NET/Posix
endif

VPATH = src src/arch/Posix src/arch/NET/$(SODISP) src/DiskIo/posix examples/MakoServer/src

SOURCE = BAS.c ThreadLib.c SoDisp.c BaFile.c MakoMain.c

endif

# Add common macros.
CFLAGS += $(D)MAKO $(D)USE_EMBEDDED_ZIP=0

ifeq ($(DEBUG),1)
CFLAGS += $(DEBUG_CFLAGS)
else
CFLAGS += $(RELEASE_CFLAGS)
endif

# Add features.
CFLAGS += $(D)USE_LUAINTF=1
CFLAGS += $(D)USE_DBGMON=1

ifeq ($(USE_OPCUA),1)
CFLAGS += $(D)USE_OPCUA=1
else
CFLAGS += $(D)USE_OPCUA=0
endif

#Do we have SQLite?
ifneq (,$(wildcard src/sqlite3.c))
$(info Including SQLite)
SOURCE += ls_sqlite3.c luasql.c sqlite3.c
else
$(info Excluding SQLite)
CFLAGS += $(D)USE_SQL=0
endif

#Do we have LPEG?
#git clone https://github.com/roberto-ieru/LPeg.git
ifneq (,$(wildcard $(LPEGDIR)/lpcode.c))
$(info Including LPEG)
CFLAGS += $(D)USE_LPEG=1
SOURCE += $(notdir $(wildcard $(LPEGDIR)/*.c))
VPATH += $(LPEGDIR)
else
$(info Excluding LPEG)
endif

#Do we have Lua Protobuf?
#https://github.com/starwing/lua-protobuf
ifneq (,$(wildcard $(PROTOBUFDIR)/pb.c))
$(info Including Lua Protobuf)
CFLAGS += $(D)USE_PROTOBUF=1
SOURCE += pb.c
VPATH += $(PROTOBUFDIR)
else
$(info Excluding Lua Protobuf)
endif

#Do we have CBOR?
#https://github.com/spc476/CBOR
ifneq (,$(wildcard $(CBORDIR)/cbor_c.c))
$(info Including CBOR)
CFLAGS += $(D)USE_CBOR=1
SOURCE += cbor_c.c dnf.c
VPATH += $(CBORDIR)
else
$(info Excluding Lua CBOR)
endif

ifneq (,$(wildcard MyCustomBindings.c))
$(info including the Lua MyCustomBindings example)
VPATH += .
SOURCE += MyCustomBindings.c MyCustomBindings_wrap.c
CFLAGS += $(D)myCustomBindings=luaopen_cpu
else
$(info Excluding Lua MyCustomBindings example)
endif

ENCRYPTION_KEY_HEADER = examples/MakoServer/src/NewEncryptionKey.h
ifeq ($(WINDOWS),1)
PYTHON := $(shell cmd /c "where python 2>NUL")
ifneq ($(PYTHON),)
CFLAGS += $(D)NewEncryptionKey
endif
else
PYTHON := $(shell command -v python3 2>/dev/null)
ifneq ($(PYTHON),)
CFLAGS += $(D)NewEncryptionKey
endif
endif


OBJS = $(SOURCE:%.c=%.$(OBJEXT))

$(TARGET): $(ENCRYPTION_KEY_HEADER) $(OBJS) mako.zip
ifeq ($(WINDOWS),1)
	$(CC) /nologo /Fe$@ $(OBJS) $(XLIB)
else
	$(CC) -o mako $(HEXEFLAGS) $(OBJS) $(XLIB)
endif

# Implicit rules for making object files from .c files
%.obj : %.c
	$(CC) $(CFLAGS) /c /Fo$@ $<

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Must be in the same directory as the mako executable
mako.zip:
ifeq ($(WINDOWS),1)
	cmd /c "cd /d ..\BAS-Resources\build && echo n|mako.cmd"
	cmd /c "copy /Y ..\BAS-Resources\build\mako.zip ."
else
	cd ../BAS-Resources/build&&./mako.sh
	cp ../BAS-Resources/build/mako.zip .
endif

MyCustomBindings_wrap.c : MyCustomBindings.i
	swig -lua MyCustomBindings.i

clean:
ifeq ($(WINDOWS),1)
	del /Q mako.exe *.obj 2>NUL || exit 0
else
	rm -f mako *.o
endif

$(ENCRYPTION_KEY_HEADER):
ifeq ($(WINDOWS),1)
ifneq ($(PYTHON),)
	@echo Generating $(ENCRYPTION_KEY_HEADER)...
	@python examples/MakoServer/GenNewEncryptionKey.py
else
	$(warning "Python is not installed. NewEncryptionKey.h not generated")
endif
else
ifneq ($(PYTHON),)
	@echo "Generating $(ENCRYPTION_KEY_HEADER)..."
	@python3 examples/MakoServer/GenNewEncryptionKey.py
else
	$(warning "Python3 is not installed. NewEncryptionKey.h not generated")
endif
endif

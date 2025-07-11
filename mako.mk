# Makefile for compiling the Mako Server for Linux
# Example: make -f mako.mk EPOLL=true
# The above compiles mako using the 'epoll' socket dispatcher. The
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

#Required
ifeq (,$(wildcard ../BAS-Resources/build/mako.sh))
$(error ../BAS-Resources not found. Repository https://github.com/RealTimeLogic/BAS-Resources required!)
endif


#Optional
export LPEGDIR=../LPeg
export PROTOBUFDIR=../lua-protobuf
export CBORDIR=../CBOR

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	XLIB= -lpthread -lm -ldl -framework IOKit -framework CoreFoundation;
	CFLAGS += -D_OSX_ -DLUA_USE_MACOSX
endif

ifndef XLIB
XLIB= -lpthread -lm -ldl 
endif

CFLAGS += -fmerge-all-constants -O3 -Os -Wall

# Add required macros and enable large-file support
CFLAGS += -DNDEBUG -DMAKO -DUSE_EMBEDDED_ZIP=0 -DLUA_USE_LINUX -DBA_FILESIZE64

# Add features
CFLAGS += -DUSE_LUAINTF=1
CFLAGS += -DUSE_DBGMON=1
CFLAGS += -DUSE_OPCUA=1

CFLAGS += -Iinc -Iinc/arch/Posix
ifdef EPOLL
SODISP = epoll
CFLAGS += -Iinc/arch/NET/epoll
else
SODISP = generic
CFLAGS += -Iinc/arch/NET/Posix
endif

VPATH = src:src/arch/Posix:src/arch/NET/$(SODISP):src/DiskIo/posix:examples/MakoServer/src

# Implicit rules for making .o files from .c files
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

SOURCE = BAS.c ThreadLib.c SoDisp.c BaFile.c MakoMain.c

#Do we have SQLite?
ifneq (,$(wildcard src/sqlite3.c))
$(info Including SQLite)
SOURCE += ls_sqlite3.c luasql.c sqlite3.c
else
$(info Excluding SQLite)
CFLAGS += -DUSE_SQL=0
endif

#Do we have LPEG?
#git clone https://github.com/roberto-ieru/LPeg.git
ifneq (,$(wildcard $(LPEGDIR)/lpcode.c))
$(info Including LPEG)
CFLAGS += -DUSE_LPEG=1
SOURCE += $(notdir $(wildcard $(LPEGDIR)/*.c))
VPATH := $(VPATH):$(LPEGDIR)
else
$(info Excluding LPEG)
endif

#Do we have Lua Protobuf?
#https://github.com/starwing/lua-protobuf
ifneq (,$(wildcard $(PROTOBUFDIR)/pb.c))
$(info Including Lua Protobuf)
CFLAGS += -DUSE_PROTOBUF=1
SOURCE += pb.c
VPATH := $(VPATH):$(PROTOBUFDIR)
else
$(info Excluding Lua Protobuf)
endif

#Do we have CBOR?
#https://github.com/spc476/CBOR
ifneq (,$(wildcard $(CBORDIR)/cbor_c.c))
$(info Including CBOR)
CFLAGS += -DUSE_CBOR=1
SOURCE += cbor_c.c dnf.c
VPATH := $(VPATH):$(CBORDIR)
else
$(info Excluding Lua CBOR)
endif

ifneq (,$(wildcard MyCustomBindings.c))
$(info including the Lua MyCustomBindings example)
VPATH += .
SOURCE += MyCustomBindings.c MyCustomBindings_wrap.c
CFLAGS += -DmyCustomBindings=luaopen_cpu
else
$(info Excluding Lua MyCustomBindings example)
endif

ENCRYPTION_KEY_HEADER = examples/MakoServer/src/NewEncryptionKey.h
PYTHON := $(shell command -v python3 2>/dev/null)
ifneq ($(PYTHON),)
CFLAGS += -DNewEncryptionKey
endif


OBJS = $(SOURCE:%.c=%.o)

mako: $(ENCRYPTION_KEY_HEADER) $(OBJS) mako.zip
	$(CC) -o mako $(OBJS) $(XLIB)

# Must be in the same directory as the mako executable
mako.zip:
	cd ../BAS-Resources/build&&./mako.sh
	cp ../BAS-Resources/build/mako.zip .

MyCustomBindings_wrap.c : MyCustomBindings.i
	swig -lua MyCustomBindings.i

clean:
	rm -f mako *.o

$(ENCRYPTION_KEY_HEADER):
ifneq ($(PYTHON),)
	@echo "Generating $(ENCRYPTION_KEY_HEADER)..."
	@python3 examples/MakoServer/GenNewEncryptionKey.py
else
	$(warning "Python3 is not installed. NewEncryptionKey.h not generated")
endif

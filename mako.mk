# Makefile for compiling the Mako Server for Linux, Mac, QNX, Windows
# with GNU Make Example:
# make -f mako.mk BA_USE_EPOLL=1
# Epoll is recommended for Linux. QNXBuild.sh selects BA_USE_POLL=1;
# other compatible POSIX builds can opt into poll with the same setting.
# Direct make defaults to generic select. EPOLL=true remains supported.
# Clean and rebuild all objects when switching dispatchers.
# SharkSSL GCC x64/ARM64 assembly defaults to auto, selected by the compiler
# target and compile checks, never the build host CPU. Failed auto checks use C.
# Inspect: make -f mako.mk sharkssl-info
# Disable: make -f mako.mk SHARKSSL_ASM=off clean
#          make -f mako.mk SHARKSSL_ASM=off
# Remove manually enabled assembly macros too for a C-only build.
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
.DEFAULT_GOAL := $(TARGET)
ODIR ?= .
BASRESDIR ?= ../BAS-Resources

ifeq (,$(wildcard $(BASRESDIR)/build))
$(error $(BASRESDIR) not found. Set BASRESDIR to the https://github.com/RealTimeLogic/BAS-Resources checkout!)
endif

BA_ENABLE_LUA_OPCUA?=1
DEBUG?=0

# Keep the existing EPOLL option compatible with earlier build commands.
BA_USE_POLL ?= 0
BA_USE_EPOLL ?= $(if $(EPOLL),1,0)
ifneq ($(BA_USE_POLL),0)
ifneq ($(BA_USE_POLL),1)
$(error BA_USE_POLL must be 0 or 1)
endif
endif
ifneq ($(BA_USE_EPOLL),0)
ifneq ($(BA_USE_EPOLL),1)
$(error BA_USE_EPOLL must be 0 or 1)
endif
endif
ifeq ($(BA_USE_POLL)$(BA_USE_EPOLL),11)
$(error Select only one of BA_USE_POLL and BA_USE_EPOLL)
endif
ifeq ($(OS),QNX)
ifeq ($(BA_USE_EPOLL),1)
$(error BA_USE_EPOLL requires Linux; use BA_USE_POLL=1 for QNX)
endif
endif

#Optional
export LPEGDIR=../LPeg
export PROTOBUFDIR=../lua-protobuf
export CBORDIR=../CBOR

ifeq ($(WINDOWS),1)

ifneq ($(filter 1,$(BA_USE_POLL) $(BA_USE_EPOLL)),)
$(error Poll and epoll require a compatible POSIX target)
endif

XLIB=ws2_32.lib advapi32.lib user32.lib shell32.lib
D=/D
I=/I
DEBUG_CFLAGS=/Od /Zi $(D)_DEBUG
RELEASE_CFLAGS=/O2 $(D)NDEBUG
CFLAGS += /nologo /W3

# Add required macros and Windows configuration.
CFLAGS += $(D)BA_BYTE_ORDER=BA_BYTE_ORDER_LITTLE_ENDIAN $(D)_CRT_SECURE_NO_WARNINGS $(D)WIN32_LEAN_AND_MEAN=1 $(D)WIN32 $(D)_CONSOLE

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
CFLAGS += $(D)LUA_USE_LINUX $(D)BA_FILE_OFFSET_BITS=64

CFLAGS += $(I)inc $(I)inc/arch/Posix
ifeq ($(BA_USE_POLL),1)
SODISP = poll
CFLAGS += $(I)inc/arch/NET/poll $(D)BA_USE_POLL=1
else
ifeq ($(BA_USE_EPOLL),1)
SODISP = epoll
CFLAGS += $(I)inc/arch/NET/epoll $(D)BA_USE_EPOLL=1
else
SODISP = generic
CFLAGS += $(I)inc/arch/NET/Posix
endif
endif

VPATH = src src/arch/Posix src/arch/NET/$(SODISP) src/DiskIo/posix examples/MakoServer/src

SOURCE = BAS.c ThreadLib.c SoDisp.c BaFile.c MakoMain.c MakoPosixMain.c

endif

# Add common macros.
CFLAGS += $(D)MAKO $(D)BA_ENABLE_MDNS=1 $(D)BA_ENABLE_MDNS_LUA=1 $(D)MAKO_ENABLE_EMBEDDED_ZIP_FALLBACK=0

# The responder core is in BAS.c; its socket adapter is platform-specific.
SOURCE += BamDNS-Sock.c
VPATH += src/arch/NET

ifeq ($(DEBUG),1)
CFLAGS += $(DEBUG_CFLAGS)
else
CFLAGS += $(RELEASE_CFLAGS)
endif

# Add features.
CFLAGS += $(D)MAKO_ENABLE_LUA_MODULE_INTERFACE=1
CFLAGS += $(D)BA_ENABLE_LUA_DEBUGGER=1

ifeq ($(BA_ENABLE_LUA_OPCUA),1)
CFLAGS += $(D)BA_ENABLE_LUA_OPCUA=1
else
CFLAGS += $(D)BA_ENABLE_LUA_OPCUA=0
endif

#Do we have SQLite?
ifneq (,$(wildcard src/sqlite3.c))
$(info Including SQLite)
SOURCE += ls_sqlite3.c luasql.c sqlite3.c
else
$(info Excluding SQLite)
CFLAGS += $(D)MAKO_SQL_BACKEND=MAKO_SQL_NONE
endif

#Do we have LPEG?
#git clone https://github.com/roberto-ieru/LPeg.git
ifneq (,$(wildcard $(LPEGDIR)/lpcode.c))
$(info Including LPEG)
CFLAGS += $(D)BA_ENABLE_LUA_LPEG=1
SOURCE += $(notdir $(wildcard $(LPEGDIR)/*.c))
VPATH += $(LPEGDIR)
else
$(info Excluding LPEG)
endif

#Do we have Lua Protobuf?
#https://github.com/starwing/lua-protobuf
ifneq (,$(wildcard $(PROTOBUFDIR)/pb.c))
$(info Including Lua Protobuf)
CFLAGS += $(D)BA_ENABLE_LUA_PROTOBUF=1
SOURCE += pb.c
VPATH += $(PROTOBUFDIR)
else
$(info Excluding Lua Protobuf)
endif

#Do we have CBOR?
#https://github.com/spc476/CBOR
ifneq (,$(wildcard $(CBORDIR)/cbor_c.c))
$(info Including CBOR)
CFLAGS += $(D)BA_ENABLE_LUA_CBOR=1
SOURCE += cbor_c.c dnf.c
VPATH += $(CBORDIR)
else
$(info Excluding Lua CBOR)
endif

ifneq (,$(wildcard MyCustomBindings.c))
$(info including the Lua MyCustomBindings example)
VPATH += .
SOURCE += MyCustomBindings.c MyCustomBindings_wrap.c
CFLAGS += $(D)MAKO_OPEN_BINDINGS=luaopen_cpu
else
$(info Excluding Lua MyCustomBindings example)
endif

ENCRYPTION_KEY_HEADER = examples/MakoServer/src/NewEncryptionKey.h
ifeq ($(WINDOWS),1)
PYTHON := $(shell cmd /c "where python 2>NUL")
ifneq ($(PYTHON),)
CFLAGS += $(D)MAKO_SOFTTPM_KEY_HEADER=\"NewEncryptionKey.h\"
endif
else
PYTHON := $(shell command -v python3 2>/dev/null)
ifneq ($(PYTHON),)
CFLAGS += $(D)MAKO_SOFTTPM_KEY_HEADER=\"NewEncryptionKey.h\"
endif
endif


# Reuse the SDK selection/probe/assembly rules with the public package layout.
# Only the GCC backends are shipped here; other compiler targets retain C.
SHARKSSL_BACKENDS := X64_GCC ARM64_GCC
SHARKSSL_CRYPTO_DIR := src/crypto
SHARKSSL_INCLUDE_DIR := inc
MFT = $(D)
O = .$(OBJEXT)
include SharkSslAsm.mk

OBJS = $(SOURCE:%.c=$(ODIR)/%.$(OBJEXT)) $(SHARKSSL_ASM_OBJS)
$(OBJS): $(SHARKSSL_STAMP)
$(ODIR)/BAS.$(OBJEXT): src/crypto/SharkSslCryptoASM.h
$(ODIR)/MakoMain.$(OBJEXT): $(ENCRYPTION_KEY_HEADER)
$(OBJS) $(SHARKSSL_STAMP): | $(ODIR)

$(ODIR):
ifeq ($(WINDOWS),1)
	@if not exist "$(ODIR)" mkdir "$(ODIR)"
else
	mkdir -p $(ODIR)
endif

$(TARGET): $(ENCRYPTION_KEY_HEADER) $(OBJS) mako.zip
ifeq ($(WINDOWS),1)
	$(CC) /nologo /Fe$@ $(OBJS) $(XLIB)
else
	$(CC) -o $@ $(HEXEFLAGS) $(OBJS) $(XLIB)
endif

# Implicit rules for making object files from .c files
$(ODIR)/%.obj : %.c
	$(CC) $(CFLAGS) /c /Fo$@ $<

$(ODIR)/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Must be in the same directory as the mako executable
mako.zip:
ifeq ($(WINDOWS),1)
	cmd /c "cd /d $(subst /,\,$(BASRESDIR))\build && echo n|mako.cmd"
	cmd /c "copy /Y $(subst /,\,$(BASRESDIR))\build\mako.zip ."
else
	cd $(BASRESDIR)/build&&./mako.sh
	cp $(BASRESDIR)/build/mako.zip .
endif

MyCustomBindings_wrap.c : MyCustomBindings.i
	swig -lua MyCustomBindings.i

.PHONY: clean
clean:
ifeq ($(WINDOWS),1)
	del /Q $(subst /,\,$(TARGET)) $(subst /,\,$(ODIR))\*.obj $(subst /,\,$(ODIR))\.sharkssl-* 2>NUL || exit 0
else
	rm -f $(TARGET) $(ODIR)/*.o $(ODIR)/.sharkssl-*
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

# Makefile for compiling the Mako Server for Linux
# Example: make -f mako.mk EPOLL=true
# The above compiles mako using the 'epoll' socket dispatcher. The
# default is to use the 'select' socket dispatcher.

# The makefile is designed for the "Embedded Linux Web Based Device
# Management" tutorial and will auto include the generated Lua
# bindings if found. The makefile will also auto include SQLite, Lua
# Protobuf, and LPeg if found. LPeg can be installed as follows:
# wget http://www.inf.puc-rio.br/~roberto/lpeg/lpeg-1.0.2.tar.gz
# cd src
# tar xvzf ../lpeg-1.0.2.tar.gz
# mv lpeg-1.0.2 lpeg

CFLAGS += -fmerge-all-constants -O3 -Os -Wall

# Add required macros and enable large-file support
CFLAGS += -DMAKO -DUSE_EMBEDDED_ZIP=0 -DLUA_USE_LINUX -DBA_FILESIZE64

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
ifneq (,$(wildcard src/lpeg/lpcode.c))
$(info Including LPEG)
CFLAGS += -DUSE_LPEG=1
SOURCE += $(notdir $(wildcard src/lpeg/*.c))
VPATH += src/lpeg
else
$(info Excluding LPEG)
endif

#Do we have Lua Protobuf?
#https://github.com/starwing/lua-protobuf
ifneq (,$(wildcard src/lua-protobuf/pb.c))
$(info Including Lua Protobuf)
CFLAGS += -DUSE_PROTOBUF=1
SOURCE += src/lua-protobuf/pb.c
VPATH += src/lua-protobuf
else
$(info Excluding Lua Protobuf)
endif

ifneq (,$(wildcard MyCustomBindings.c))
$(info including the Lua MyCustomBindings example)
VPATH += .
SOURCE += MyCustomBindings.c MyCustomBindings_wrap.c
CFLAGS += -DmyCustomBindings=luaopen_cpu
else
$(info Excluding Lua MyCustomBindings example)
endif

OBJS = $(SOURCE:%.c=%.o)

mako: $(OBJS) mako.zip
	$(CC) -o mako $(OBJS) -lpthread -lm -ldl

# Must be in the same directory as the mako executable
mako.zip: examples/MakoServer/mako.zip
	cp $< .

MyCustomBindings_wrap.c : MyCustomBindings.i
	swig -lua MyCustomBindings.i

clean:
	rm -f mako *.o




# Basic makefile for Linux

VPATH+=src:../HostInit:../../src:../../src/arch/Posix:../../src/arch/NET/generic:../../src/DiskIo/posix

CFLAGS += -c -g -Wall
CFLAGS += -DBA_FILESIZE64 -DLUA_USE_LINUX
CFLAGS += -DBAIO_DISK
CFLAGS += -I../../inc -I../../inc/arch/Posix -I../../inc/arch/NET/Posix

ifndef ODIR
ODIR = obj
endif

CPPSRC=ChatServer.cpp
CSRC=Main.c HostInit.c BWS.c ThreadLib.c SoDisp.c BaFile.c

# Implicit rules for making .o files from .c files
$(ODIR)/%.o : %.c
	gcc $(CFLAGS) -o $@ $<
# Implicit rules for making .o files from .cpp files
$(ODIR)/%.o : %.cpp
	g++ $(CFLAGS) -o $@ $<

OBJS = $(CSRC:%.c=$(ODIR)/%.o) $(CPPSRC:%.cpp=$(ODIR)/%.o)

.PHONY : all clean

all: $(ODIR) ChatServer

ChatServer: $(OBJS)
	g++ -o $@ $^  -lpthread -lm -ldl
	echo "Build complete; start server: ./ChatServer"

$(ODIR):
	mkdir $(ODIR)

clean:
	rm -rf $(ODIR) ChatServer



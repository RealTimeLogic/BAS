# Build the packaged C example with GNU Make and GCC, or MSVC on Windows.
# Windows make builds also need the usual GNU mkdir/rm utilities on PATH.
BAROOT ?= $(abspath ../../..)
build ?= release
ODIR ?= ../obj/$(build)
SHARKTRUSTX_PRIVATE_CA ?= 0
# An optional target file supplies cross compiler, port sources and includes.
ifdef tinc
include $(tinc)
endif
plat ?= $(if $(filter Windows_NT,$(OS)),win,posix)
.DEFAULT_GOAL := exe

ifeq ($(plat),win)
ifeq ($(origin CC),default)
CC = cl
endif
ifeq ($(origin AR),default)
AR = lib
endif
ARC ?= x64
O = .obj
EXE = .exe
GLIBNAME = GLib.lib
D = /D
I = /I
OFT = /Fo
PORTSRC ?= $(BAROOT)/src/arch/Windows/ThreadLib.c $(BAROOT)/src/arch/NET/generic/SoDisp.c $(BAROOT)/src/DiskIo/windows/BaFile.c
PORTINC ?= $(BAROOT)/inc/arch/Windows $(BAROOT)/inc/arch/NET/Windows
CPPFLAGS += /DB_LITTLE_ENDIAN /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS
CFLAGS += /nologo /W3 /TC /Gy /Fd$(ODIR)/compiler.pdb
ifeq ($(build),debug)
CPPFLAGS += /D_DEBUG
CFLAGS += /Od /Zi /MDd /RTC1
else
CPPFLAGS += /DNDEBUG
CFLAGS += /O2 /MD
endif
LDLIBS += ws2_32.lib bcrypt.lib
ARCHIVE = $(AR) /nologo /OUT:$@ $(GOBJS)
LINK = $(CC) /nologo /Fe$@ $(HOBJS) $(GLIB) $(LDLIBS) /link $(LDFLAGS) /INCREMENTAL:NO /MACHINE:$(if $(filter x64,$(ARC)),X64,X86)
ifeq ($(build),debug)
LDFLAGS += /DEBUG /PDB:$(ODIR)/SharkTrustX-staging.pdb
endif
else
O = .o
EXE =
GLIBNAME = libGLib.a
D = -D
I = -I
OFT = -o
ifeq ($(plat),posix)
PORTSRC ?= $(BAROOT)/src/arch/Posix/ThreadLib.c $(BAROOT)/src/arch/NET/generic/SoDisp.c $(BAROOT)/src/DiskIo/posix/BaFile.c
PORTINC ?= $(BAROOT)/inc/arch/Posix $(BAROOT)/inc/arch/NET/Posix
LDLIBS += -lpthread -lm -ldl
endif
CPPFLAGS += -DBA_FILESIZE64
CFLAGS += -Wall -MMD -MP
ifeq ($(build),debug)
CPPFLAGS += -D_DEBUG
CFLAGS += -g -O0
else
CPPFLAGS += -DNDEBUG
CFLAGS += -Os
endif
ARCHIVE = $(AR) rcs $@ $(GOBJS)
LINK = $(CC) $(LDFLAGS) -o $@ $(HOBJS) $(GLIB) $(LDLIBS)
endif

# BWS includes these services under SHARKTRUSTX; compile only one library.
CPPFLAGS += $(D)SHARKTRUSTX $(D)BA_JSON=1 $(D)TOKENGEN_C_API
CPPFLAGS += $(D)SHARKSSL_ENABLE_CERTINFO_PARSE_API=1
CPPFLAGS += $(I)../src $(I)$(BAROOT)/inc $(addprefix $(I),$(PORTINC))
ifeq ($(USE_IPV6),1)
CPPFLAGS += $(D)USE_IPV6
endif

# The private profile adds the mDNS core and one socket/discovery adapter.
ifeq ($(SHARKTRUSTX_PRIVATE_CA),1)
CPPFLAGS += $(D)SHARKTRUSTX_PRIVATE_CA
MDNSSRC ?= $(BAROOT)/src/arch/NET/BamDNS-Sock.c
MDNSSOURCE = BamDNS.c $(notdir $(MDNSSRC))
ifeq ($(plat),win)
LDLIBS += iphlpapi.lib
endif
endif

VPATH = ../src:$(BAROOT)/src:$(BAROOT)/examples/HostInit
VPATH += $(sort $(dir $(PORTSRC) $(MDNSSRC)))
GSOURCE = BWS.c start.c InstallVD.c tokengen.c $(notdir $(PORTSRC)) $(MDNSSOURCE)
HSOURCE = Main.c HostInit.c
GOBJS = $(addprefix $(ODIR)/,$(GSOURCE:.c=$(O)))
HOBJS = $(addprefix $(ODIR)/,$(HSOURCE:.c=$(O)))
GLIB = $(ODIR)/$(GLIBNAME)
HEXE = $(ODIR)/SharkTrustX-staging$(EXE)

.PHONY: exe lib clean
exe: $(HEXE)
# An RTOS supplies its own startup and calls barracuda() from a server task.
lib: $(GLIB)

$(ODIR):
	mkdir -p $(ODIR)

$(GOBJS) $(HOBJS): | $(ODIR)
$(GOBJS) $(HOBJS): Makefile $(wildcard ../src/*.h $(BAROOT)/inc/*.h)
$(GOBJS) $(HOBJS): $(BAROOT)/examples/HostInit/ExampleStartup.h

$(ODIR)/%$(O): %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(OFT)$@ $<

$(GLIB): $(GOBJS)
	rm -f $@
	$(ARCHIVE)

$(HEXE): $(HOBJS) $(GLIB)
	$(LINK)

# Clean with the old settings before switching profiles, compilers or ports.
# Saved enrollment and certificate files are deliberately preserved.
clean:
	rm -f $(GOBJS) $(HOBJS) $(GLIB) $(HEXE) $(ODIR)/*.d $(ODIR)/*.pdb $(ODIR)/*.ilk

ifneq ($(plat),win)
-include $(GOBJS:.o=.d) $(HOBJS:.o=.d)
endif

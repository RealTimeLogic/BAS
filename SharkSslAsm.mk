# Detect the compiler TARGET, never the host. No target executable is run.
# Copyright (c) Real Time Logic LLC, 2026. All rights reserved.
include SharkSslBackends.mk
SHARKSSL_ASM ?= auto
ifneq ($(words $(SHARKSSL_ASM)),1)
$(error SHARKSSL_ASM must be one of: auto off $(SHARKSSL_BACKENDS))
endif
ifeq (,$(filter $(SHARKSSL_ASM),auto off $(SHARKSSL_BACKENDS)))
$(error SHARKSSL_ASM must be one of: auto off $(SHARKSSL_BACKENDS))
endif
SHARKSSL_BACKEND := none
SHARKSSL_DETECTED_BACKEND := none
# Public amalgamated packages supply their own layout; the SDK keeps its defaults.
SHARKSSL_CRYPTO_DIR ?= $(BAROOT)/src/plugins/SharkSSL/src/crypto
SHARKSSL_INCLUDE_DIR ?= $(BAROOT)/src/plugins/SharkSSL/inc
SHARKSSL_PROBE_NULL := $(if $(filter cmd cmd.exe CMD.EXE,$(notdir $(SHELL))),NUL,/dev/null)

ifneq ($(SHARKSSL_ASM),off)
ifndef usebalibs
ifndef NO_SHARKSSL
# Probe failure selects no backend. Use exactly the C compiler and C flags.
ifeq ($(MFT),/D)
SHARKSSL_PROBE := $(shell $(CC) $(CFLAGS) /EP /I$(SHARKSSL_INCLUDE_DIR) SharkSslTarget.c 2>$(SHARKSSL_PROBE_NULL) && echo SHARKSSL_MAKE_PROBE_OK)
else
# qcc interprets -P as preprocess to a .i file, hiding the markers from make.
# -E alone writes stdout for GCC, Clang and qcc; line markers are filtered out.
SHARKSSL_PROBE := $(shell $(CC) $(CFLAGS) -E -x c -I$(SHARKSSL_INCLUDE_DIR) SharkSslTarget.c 2>$(SHARKSSL_PROBE_NULL) && echo SHARKSSL_MAKE_PROBE_OK)
endif
ifneq (,$(filter SHARKSSL_MAKE_PROBE_OK,$(SHARKSSL_PROBE)))
SHARKSSL_MATCHES := $(filter $(SHARKSSL_BACKENDS),$(patsubst SHARKSSL_MAKE_TARGET_%,%,$(filter SHARKSSL_MAKE_TARGET_%,$(SHARKSSL_PROBE))))
ifeq ($(words $(SHARKSSL_MATCHES)),1)
SHARKSSL_DETECTED_BACKEND := $(SHARKSSL_MATCHES)
endif
endif
endif
endif
endif

ifeq ($(SHARKSSL_ASM),auto)
ifeq ($(SHARKSSL_$(SHARKSSL_DETECTED_BACKEND)_AUTO),1)
SHARKSSL_BACKEND := $(SHARKSSL_DETECTED_BACKEND)
endif
else ifneq ($(SHARKSSL_ASM),off)
ifneq ($(SHARKSSL_ASM),$(SHARKSSL_DETECTED_BACKEND))
$(error Requested SharkSSL backend $(SHARKSSL_ASM) does not match the compiler target/configuration, or assembly integration is disabled)
endif
SHARKSSL_BACKEND := $(SHARKSSL_DETECTED_BACKEND)
endif

ifneq ($(SHARKSSL_BACKEND),none)
SHARKSSL_M0 := $(patsubst SHARKSSL_MAKE_M0_%,%,$(filter SHARKSSL_MAKE_M0_%,$(SHARKSSL_PROBE)))
SHARKSSL_BACKEND_DIR := $(SHARKSSL_CRYPTO_DIR)/$(SHARKSSL_$(SHARKSSL_BACKEND)_DIR)
SHARKSSL_ASFLAGS = $(SHARKSSL_$(SHARKSSL_BACKEND)_ASFLAGS)
# Test optional GAS kernels with the target toolchain, never the host CPU.
# Compile the actual source to the null device; do not run target code.
sharkssl_optional_supported = $(shell $(CC) $(CFLAGS) $(SHARKSSL_ASFLAGS) -c $(SHARKSSL_BACKEND_DIR)/$(SHARKSSL_$(SHARKSSL_BACKEND)_$(1)_ASM) -o $(SHARKSSL_PROBE_NULL) >$(SHARKSSL_PROBE_NULL) 2>&1 && echo yes)
SHARKSSL_UNSUPPORTED := $(foreach option,$(SHARKSSL_$(SHARKSSL_BACKEND)_OPTIONAL),$(if $(call sharkssl_optional_supported,$(option)),,$(option)))
ifneq (,$(filter $(addprefix SHARKSSL_MAKE_,$(addsuffix _REQUIRED,$(SHARKSSL_UNSUPPORTED))),$(SHARKSSL_PROBE)))
$(error Explicitly enabled SharkSSL option is unsupported by the target assembler: $(SHARKSSL_UNSUPPORTED))
endif
SHARKSSL_SELECTED_ASM := $(filter-out $(foreach option,$(SHARKSSL_UNSUPPORTED),$(SHARKSSL_$(SHARKSSL_BACKEND)_$(option)_ASM)),$(SHARKSSL_$(SHARKSSL_BACKEND)_ASM))
SHARKSSL_DEFAULTS := $(filter-out $(SHARKSSL_UNSUPPORTED),$(SHARKSSL_$(SHARKSSL_BACKEND)_DEFAULTS))
# A backend may default a hook only when the configured algorithm supports it.
SHARKSSL_UNAVAILABLE := $(patsubst SHARKSSL_MAKE_%_UNAVAILABLE,%,$(filter SHARKSSL_MAKE_%_UNAVAILABLE,$(SHARKSSL_PROBE)))
SHARKSSL_DEFAULTS := $(filter-out $(SHARKSSL_UNAVAILABLE),$(SHARKSSL_DEFAULTS))
# Leave an explicitly configured GCM/GHASH/VAES group to the application.
ifneq (,$(filter SHARKSSL_MAKE_GCM_DEFINED SHARKSSL_MAKE_GHASH_DEFINED,$(SHARKSSL_PROBE)))
SHARKSSL_DEFAULTS := $(filter-out GCM GHASH GCM_VAES,$(SHARKSSL_DEFAULTS))
endif
sharkssl_option = $(if $(SHARKSSL_OPTION_$(1)),$(SHARKSSL_OPTION_$(1)),SHARKSSL_OPTIMIZED_$(1)_ASM)
SHARKSSL_AUTO_FLAGS := $(foreach option,$(SHARKSSL_DEFAULTS),$(if $(filter SHARKSSL_MAKE_$(option)_DEFINED,$(SHARKSSL_PROBE)),,$(MFT)$(call sharkssl_option,$(option))=1))
# Check target C helpers as well as kernels, using the final feature flags.
# Otherwise a guarded .S file could compile as empty and falsely pass.
# Like the optional x64 check, compile to the null device; never run target code.
ifeq ($(SHARKSSL_$(SHARKSSL_BACKEND)_COMPILE_CHECK),1)
sharkssl_source_supported = $(shell $(CC) $(CFLAGS) $(SHARKSSL_AUTO_FLAGS) $(SHARKSSL_ASFLAGS) -c $(SHARKSSL_BACKEND_DIR)/$(1) -o $(SHARKSSL_PROBE_NULL) >$(SHARKSSL_PROBE_NULL) 2>&1 && echo yes)
SHARKSSL_COMPILE_FAILURES := $(foreach source,$(SHARKSSL_SELECTED_ASM) $(SHARKSSL_$(SHARKSSL_BACKEND)_C),$(if $(call sharkssl_source_supported,$(source)),,$(source)))
ifneq ($(strip $(SHARKSSL_COMPILE_FAILURES)),)
ifneq ($(SHARKSSL_ASM),auto)
$(error Requested SharkSSL backend $(SHARKSSL_BACKEND) failed target compile checks: $(SHARKSSL_COMPILE_FAILURES))
endif
ifneq (,$(filter SHARKSSL_MAKE_%_REQUIRED,$(SHARKSSL_PROBE)))
$(error Explicit SharkSSL assembly settings cannot be satisfied; target compile checks failed: $(SHARKSSL_COMPILE_FAILURES))
endif
$(warning SharkSSL $(SHARKSSL_BACKEND) compile checks failed; using C: $(SHARKSSL_COMPILE_FAILURES))
SHARKSSL_BACKEND := none
SHARKSSL_AUTO_FLAGS :=
SHARKSSL_SELECTED_ASM :=
SHARKSSL_M0 :=
endif
endif

ifneq ($(SHARKSSL_BACKEND),none)
override CFLAGS += $(SHARKSSL_AUTO_FLAGS)
VPATH := $(VPATH):$(SHARKSSL_BACKEND_DIR)
SOURCE += $(SHARKSSL_$(SHARKSSL_BACKEND)_C)
SHARKSSL_ASM_NAMES := $(basename $(SHARKSSL_SELECTED_ASM))
SHARKSSL_ASM_OBJS := $(addprefix $(ODIR)/sharkssl_,$(addsuffix $(O),$(SHARKSSL_ASM_NAMES)))
ifeq ($(SHARKSSL_$(SHARKSSL_BACKEND)_DRIVER),cc)
$(SHARKSSL_ASM_OBJS): $(ODIR)/sharkssl_%$(O): $(SHARKSSL_BACKEND_DIR)/%.S
	$(CC) $(CFLAGS) $(SHARKSSL_ASFLAGS) -c -o $@ $<
else ifeq ($(SHARKSSL_$(SHARKSSL_BACKEND)_DRIVER),masm)
SHARKSSL_MASM ?= $(SHARKSSL_$(SHARKSSL_BACKEND)_TOOL)
$(SHARKSSL_ASM_OBJS): $(ODIR)/sharkssl_%$(O): $(SHARKSSL_BACKEND_DIR)/%.asm
	$(SHARKSSL_MASM) /nologo /c $(SHARKSSL_ASFLAGS) /Fo$@ $<
else
$(error Unknown assembler driver for $(SHARKSSL_BACKEND))
endif
endif
endif

.PHONY: sharkssl-info
sharkssl-info:
	@echo SharkSSL detected backend: $(SHARKSSL_DETECTED_BACKEND)
	@echo SharkSSL backend: $(SHARKSSL_BACKEND)
	@echo SharkSSL automatic defines: $(SHARKSSL_AUTO_FLAGS)
	@echo SharkSSL AES layout: $(SHARKSSL_M0)
	@echo SharkSSL assembly files: $(SHARKSSL_SELECTED_ASM)
	@echo SharkSSL unsupported optional assembly: $(SHARKSSL_UNSUPPORTED)
	@echo SharkSSL failed backend compile checks: $(SHARKSSL_COMPILE_FAILURES)
ifeq ($(SHARKSSL_ASM),auto)
ifeq ($(SHARKSSL_$(SHARKSSL_DETECTED_BACKEND)_AUTO),0)
	@echo Enable for target validation with SHARKSSL_ASM=$(SHARKSSL_DETECTED_BACKEND)
endif
endif

# Rebuild C as well as assembly after a backend/layout or description change.
# Other compiler/command-line flag changes still require clean/separate ODIRs.
SHARKSSL_STAMP := $(ODIR)/.sharkssl-$(SHARKSSL_BACKEND)-$(SHARKSSL_M0)
$(SHARKSSL_STAMP): SharkSslAsm.mk SharkSslBackends.mk SharkSslTarget.c $(SHARKSSL_INCLUDE_DIR)/SharkSSL_opts.h $(SHARKSSL_INCLUDE_DIR)/SharkSSL_cfg.h
ifeq ($(SHARKSSL_PROBE_NULL),NUL)
	@if exist "$(subst /,\,$(ODIR))\.sharkssl-*" del /Q "$(subst /,\,$(ODIR))\.sharkssl-*"
else
	$(RM) $(ODIR)/.sharkssl-*
endif
	@echo $(SHARKSSL_BACKEND) > $@

CC := gcc
CXX := g++
HOST_CC ?= cc
NODE ?= node
NPM ?= npm
ZIP ?= zip
AWK ?= awk

BUILD_DIR ?= build/posix
TARGET ?= restservice
RELEASE_BUILD_DIR ?= build/posix-release
RELEASE_TARGET ?= restservice-release

CLIENT_DIR := client
CLIENT_DIST_DIR := $(CLIENT_DIR)/dist
CLIENT_NODE_MODULES_STAMP := $(CLIENT_DIR)/node_modules/.package-lock.json

BIN2C := $(RELEASE_BUILD_DIR)/bin2c
EMBEDDED_UI_ZIP := $(RELEASE_BUILD_DIR)/device-console.zip
EMBEDDED_UI_C := src/EmbeddedUi.c
BIN2C_UI_C := $(RELEASE_BUILD_DIR)/EmbeddedUi.bin2c.c
GENERATED_UI_C := $(RELEASE_BUILD_DIR)/EmbeddedUi.generated.c
NPM_LOG := $(RELEASE_BUILD_DIR)/npm.log

define run_npm
	@if command -v $(NODE) >/dev/null 2>&1; then \
		if ! $(NODE) -e 'const v=process.versions.node.split(".").map(Number); process.exit((v[0] === 20 && v[1] >= 19) || (v[0] >= 22 && (v[0] > 22 || v[1] >= 12)) ? 0 : 1)'; then \
			echo "Node.js $$($(NODE) --version) is too old for this client."; \
			echo "Vite 8 requires Node.js ^20.19.0 or >=22.12.0."; \
			exit 2; \
		fi; \
		$(NPM) --prefix $(CLIENT_DIR) $(1); \
	elif command -v cmd.exe >/dev/null 2>&1 && \
	     cmd.exe /d /c "where npm.cmd >nul 2>nul"; then \
		cmd.exe /d /c "npm.cmd --prefix $(CLIENT_DIR) $(1) > $(NPM_LOG) 2>&1"; \
		status=$$?; \
		cat $(NPM_LOG); \
		$(RM) $(NPM_LOG); \
		exit $$status; \
	else \
		echo "Node.js/npm is not usable in this environment."; \
		echo "Install Node.js ^20.19.0 or >=22.12.0 in Linux,"; \
		echo "or expose Windows node.exe and npm.cmd to WSL."; \
		exit 127; \
	fi
endef

CPPFLAGS += -DBA_FILESIZE64 -DCHANGE_2_OBJ_DIR=0 \
            -I../../inc \
            -I../../inc/arch/Posix \
            -I../../inc/arch/NET/Posix \
            -I../../cpp/rest/inc
CFLAGS ?= -O2
CFLAGS += -Wall -Wextra -MMD -MP -pthread
CXXFLAGS ?= -O2
CXXFLAGS += -std=c++11 -Wall -Wextra -MMD -MP -pthread \
            -fno-exceptions -fno-rtti
HOST_CFLAGS ?= -O2 -Wall -Wextra
LDLIBS += -pthread -lm -ldl

RELEASE_CPPFLAGS = $(CPPFLAGS) -DBAS_REST_EMBEDDED_UI

C_SOURCES := \
    ../../src/BWS.c \
    ../../src/arch/Posix/ThreadLib.c \
    ../../src/arch/NET/generic/SoDisp.c \
    ../HostInit/Main.c \
    ../HostInit/HostInit.c

RELEASE_C_SOURCES := $(C_SOURCES) $(EMBEDDED_UI_C)

CXX_SOURCES := \
    ../../cpp/rest/src/Rest.cpp \
    src/RestExample.cpp

C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
CXX_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.o)))
OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS)
DEPENDENCIES := $(OBJECTS:.o=.d)

RELEASE_C_OBJECTS := \
    $(addprefix $(RELEASE_BUILD_DIR)/,$(notdir $(RELEASE_C_SOURCES:.c=.o)))
RELEASE_CXX_OBJECTS := \
    $(addprefix $(RELEASE_BUILD_DIR)/,$(notdir $(CXX_SOURCES:.cpp=.o)))
RELEASE_OBJECTS := $(RELEASE_C_OBJECTS) $(RELEASE_CXX_OBJECTS)
RELEASE_DEPENDENCIES := $(RELEASE_OBJECTS:.o=.d)

vpath %.c $(sort $(dir $(C_SOURCES) $(EMBEDDED_UI_C)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))

all: $(TARGET)

release: $(RELEASE_TARGET)

server: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(RELEASE_BUILD_DIR)/%.o: %.c | $(RELEASE_BUILD_DIR)
	$(CC) $(RELEASE_CPPFLAGS) $(CFLAGS) -c $< -o $@

$(RELEASE_BUILD_DIR)/%.o: %.cpp | $(RELEASE_BUILD_DIR)
	$(CXX) $(RELEASE_CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(CLIENT_NODE_MODULES_STAMP): \
      $(CLIENT_DIR)/package.json $(CLIENT_DIR)/package-lock.json | $(RELEASE_BUILD_DIR)
	$(call run_npm,ci)
	@test -f $@

$(BIN2C): ../../tools/bin2c.c | $(RELEASE_BUILD_DIR)
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $<

regenerate-embedded-ui: $(CLIENT_NODE_MODULES_STAMP) $(BIN2C) | $(RELEASE_BUILD_DIR)
	$(call run_npm,run build)
	find $(CLIENT_DIST_DIR) -type f -exec touch -t 198001010000 {} +
	$(RM) $(EMBEDDED_UI_ZIP) $(BIN2C_UI_C) $(GENERATED_UI_C)
	cd $(CLIENT_DIST_DIR) && find . -type f -print | LC_ALL=C sort | \
		$(ZIP) -q -X -D -9 $(abspath $(EMBEDDED_UI_ZIP)) -@
	$(BIN2C) -z getEmbeddedUiZipReader \
		$(EMBEDDED_UI_ZIP) $(BIN2C_UI_C)
	$(AWK) 'BEGIN { print "#ifdef BAS_REST_EMBEDDED_UI" } \
		{ print } END { print "#endif" }' \
		$(BIN2C_UI_C) > $(GENERATED_UI_C)
	@if cmp -s $(GENERATED_UI_C) $(EMBEDDED_UI_C); then \
		echo "$(EMBEDDED_UI_C) is already current"; \
	else \
		cp $(GENERATED_UI_C) $(EMBEDDED_UI_C); \
		echo "Updated $(EMBEDDED_UI_C)"; \
	fi

$(BUILD_DIR):
	mkdir -p $@

$(RELEASE_BUILD_DIR):
	mkdir -p $@

clean: clean-release
	$(RM) $(TARGET) $(OBJECTS) $(DEPENDENCIES)
	-rmdir $(BUILD_DIR)

clean-release:
	$(RM) $(RELEASE_TARGET) $(RELEASE_OBJECTS) $(RELEASE_DEPENDENCIES) \
		$(BIN2C_UI_C) $(GENERATED_UI_C) $(EMBEDDED_UI_ZIP) $(BIN2C) \
		$(RELEASE_BUILD_DIR)/EmbeddedUi.c $(NPM_LOG)
	-rmdir $(RELEASE_BUILD_DIR)

-include $(DEPENDENCIES) $(RELEASE_DEPENDENCIES)

.PHONY: all server release regenerate-embedded-ui clean clean-release

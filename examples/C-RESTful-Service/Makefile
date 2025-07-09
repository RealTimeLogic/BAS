# Compiler and flags
CC := gcc
CFLAGS := -Wall -MMD -MP \
          -I../../inc/ \
          -I../../inc/arch/Posix/ \
          -I../../inc/arch/NET/Posix/

# Source files
SRCS := \
    src/RestService.c \
    src/RestJsonUtils.c \
    ../../src/arch/Posix/ThreadLib.c \
    ../../src/arch/NET/generic/SoDisp.c \
    ../../src/BWS.c \
    ../HostInit/Main.c \
    ../HostInit/HostInit.c

# Object files
OBJS := $(SRCS:.c=.o)

# Dependency files
DEPS := $(OBJS:.o=.d)

# Target executable
TARGET := restservice

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# Include dependencies
-include $(DEPS)

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

.PHONY: all clean

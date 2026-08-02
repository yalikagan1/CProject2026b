# Compiler
CC := gcc

# Compiler flags
CFLAGS := -Wall -Wextra -ansi -pedantic -Iinclude

# Directories
SRC_DIR := src
BUILD_DIR := build
TARGET := program

# Find all source files recursively
SRCS := $(shell find $(SRC_DIR) -name "*.c")

# Convert source paths to object paths
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Rebuild
re: clean all

.PHONY: all clean re

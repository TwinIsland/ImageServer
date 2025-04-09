# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -I./ -Iincludes
LDFLAGS := -static -L./libs -lpthread -ldl -lm

# Directories
SRC_DIR := .
LIB_DIR := libs
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c) \
        $(wildcard $(SRC_DIR)/src/*.c) \
        $(wildcard $(SRC_DIR)/setup/*.c) \
        $(wildcard $(LIB_DIR)/*.c)

# Object files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%.c,$(SRCS))) \
        $(patsubst $(SRC_DIR)/src/%.c,$(OBJ_DIR)/src/%.o,$(filter $(SRC_DIR)/src/%.c,$(SRCS))) \
        $(patsubst $(SRC_DIR)/setup/%.c,$(OBJ_DIR)/setup/%.o,$(filter $(SRC_DIR)/setup/%.c,$(SRCS))) \
        $(patsubst $(LIB_DIR)/%.c,$(OBJ_DIR)/libs/%.o,$(filter $(LIB_DIR)/%.c,$(SRCS)))

# Final executable
TARGET := $(BIN_DIR)/dispatcher

# Ensure build directories exist
$(shell mkdir -p $(OBJ_DIR)/src $(OBJ_DIR)/setup $(OBJ_DIR)/libs $(BIN_DIR))

# Default target
all: $(TARGET)

# Link target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile rule for main source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rule for src files
$(OBJ_DIR)/src/%.o: $(SRC_DIR)/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rule for setup files
$(OBJ_DIR)/setup/%.o: $(SRC_DIR)/setup/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile rule for libs files
$(OBJ_DIR)/libs/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	@find $(BUILD_DIR) -type f ! -path '$(OBJ_DIR)/libs/sqlite3.o' -delete
	@find $(BUILD_DIR) -type d -empty -delete

.PHONY: all clean
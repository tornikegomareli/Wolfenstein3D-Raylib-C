.PHONY: all clean

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./src -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Target
TARGET = $(BIN_DIR)/wolf3d-gpu

# OS detection
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    # macOS
    LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
endif
ifeq ($(UNAME), Linux)
    # Linux
    LDFLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
endif

# Default target
all: $(TARGET)

# Create directories
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: all
	$(TARGET)
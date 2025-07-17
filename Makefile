# Compiler and flags
CXX := g++
CC := gcc
CXXFLAGS := -std=c++20 -Wall -Wextra -g
CFLAGS := -Wall -Wextra -g

# Manual list of sources
SRC_CPP := main.cpp \
		   utils/utils.cpp \
		   utils/debug.cpp

SRC_C := modules/register.c

# Automatically detect all .c and .cpp files inside custom_modules/
CUSTOM_CPP := $(shell find custom_modules -name '*.cpp')
CUSTOM_C := $(shell find custom_modules -name '*.c')

# Convert source lists to object files in build/ folder
OBJ_CPP := $(patsubst %.cpp,build/%.o,$(SRC_CPP)) $(patsubst %.cpp,build/%.o,$(CUSTOM_CPP))
OBJ_C := $(patsubst %.c,build/%.o,$(SRC_C)) $(patsubst %.c,build/%.o,$(CUSTOM_C))

# Final executable
OUT := compiler

# Default target
all: $(OUT)

# Link
$(OUT): $(OBJ_CPP) $(OBJ_C)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile C++ files
build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile C files
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean
.PHONY: clean
clean:
	rm -rf $(OUT) build/
	find . -name '*.o' -delete

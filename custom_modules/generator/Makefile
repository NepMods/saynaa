# === per-module Makefile ===

# 1) Read metadata from module.info
MODULE_NAME     := $(shell grep '^name'     module.info | cut -d= -f2 | xargs)
MODULE_PRIORITY := $(shell grep '^priority' module.info | cut -d= -f2 | xargs)

CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -g -fPIC -DMODULE_NAME="\"$(MODULE_NAME)\"" -DMODULE_PRIORITY=$(MODULE_PRIORITY)

# 2) Discover all .cpp files recursively
SRCS := $(shell find . -name '*.cpp')
OBJS := $(patsubst ./%.cpp,build/%.o,$(SRCS))

OUT := lib$(MODULE_NAME).a

all: $(OUT)

$(OUT): $(OBJS)
	@ar rcs $@ $^

# 3) Compile .cpp into build/*.o
build/%.o: %.cpp
	@mkdir -p build/ # Changed to directly create 'build/' directory
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf build/*.o $(OUT)

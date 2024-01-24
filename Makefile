MODE := debug
NAME := lang
SOURCE_DIR := src
CFLAGS := -std=c++11
CC := g++

# Mode configuration.
ifeq ($(MODE),debug)
	CFLAGS += -O0 -DDEBUG -g
	BUILD_DIR := build/debug
else
	CFLAGS += -O3 -flto
	BUILD_DIR := build/release
endif

# Files.
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJECTS := $(addprefix $(BUILD_DIR)/$(NAME)/, $(notdir $(SOURCES:.cpp=.o)))

# Targets ---------------------------------------------------------------------

# Link.
$(NAME): $(OBJECTS)
	@ mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

# Compile object files.
$(BUILD_DIR)/$(NAME)/%.o: $(SOURCE_DIR)/%.cpp $(HEADERS)
	@ mkdir -p $(BUILD_DIR)/$(NAME)
	$(CC) -c $(C_LANG) $(CFLAGS) -o $@ $<

clean clear:
	@ rm -rf build
	@ rm -f $(NAME)

.PHONY: default

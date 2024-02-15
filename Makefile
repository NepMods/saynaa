
## PROGRAM NAME
NAME = saynaa

## MODE can be DEBUG or RELEASE
MODE = DEBUG

CC        := g++
CFLAGS    := -std=c++20
LDFLAGS   := -lm
OBJ_DIR   := build/

SRC  = src/cli/        \
       src/compiler/   \
       src/generator/  \
       src/shared/     \
       src/utils/      \

SRCS  := $(foreach DIR,$(SRC),$(wildcard $(DIR)*.cpp))
OBJS  := $(addprefix $(OBJ_DIR), $(SRCS:.cpp=.o))

ifeq ($(MODE),DEBUG)
	CFLAGS += -DDEBUG -g3 -Og
else
	CFLAGS += -g -O3
endif

.PHONY: all clean

$(NAME): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

install:
	@cp -r $(NAME) /usr/local/bin/
	@printf "\033[38;5;52m\033[43m\t    installed!    \t\033[0m\n";

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(NAME)

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK_NAME = libft_malloc.so
TEST_NAME = test_malloc

SRC = src/malloc.c src/helper.c src/globals.c src/block/block_utils.c \
		src/free.c src/realloc.c src/show_alloc_mem.c src/zone_cleanup.c
OBJ = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)

CC = cc
CCFLAGS = -Wall -Wextra -Werror -fPIC -MMD -MP
INCLUDES = -Iincludes -Ilibft

all: libft $(NAME) $(LINK_NAME)

$(NAME): $(OBJ) libft
	$(CC) $(CCFLAGS) -shared $(OBJ) -Llibft -lft -lpthread -o $(NAME)

$(LINK_NAME): $(NAME)
	ln -sf $(NAME) $(LINK_NAME)

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

libft:
	make -C libft

test: $(NAME) test_malloc.c
	$(CC) $(CCFLAGS) $(INCLUDES) -pthread test_malloc.c -Llibft -lft ./$(NAME) -o $(TEST_NAME)
	./$(TEST_NAME)

clean:
	rm -f $(OBJ) $(DEPS)
	make clean -C libft

fclean: clean
	rm -f $(NAME) $(LINK_NAME) $(TEST_NAME)
	make fclean -C libft

re: fclean all

.PHONY: all clean fclean re libft test

-include $(DEPS)
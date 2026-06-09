ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK_NAME = libft_malloc.so

SRC = src/malloc.c src/helper.c src/globals.c src/block/block_utils.c \
		src/free.c src/realloc.c src/show_alloc_mem.c
OBJ = $(SRC:.c=.o)

CC = cc
CCFLAGS = -Wall -Wextra -Werror -fPIC
INCLUDES = -Iincludes -Ilibft

all: libft/libft.a $(NAME) $(LINK_NAME)

$(NAME): $(OBJ) libft/libft.a
	$(CC) $(CCFLAGS) -shared $(OBJ) -Llibft -lft -lpthread -o $(NAME)

$(LINK_NAME): $(NAME)
	ln -sf $(NAME) $(LINK_NAME)

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

libft/libft.a:
	make -C libft

clean:
	rm -f $(OBJ)
	make clean -C libft

fclean: clean
	rm -f $(NAME) $(LINK_NAME)
	make fclean -C libft

re: fclean all

.PHONY: all clean fclean re libft
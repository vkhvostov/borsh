NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR=libft
LIBFT_NAME=libft.a
SRCS = src/main.c \
		src/utils.c
OBJS = $(SRCS:.c=.o)
HEADERS = includes/borsh.h

all: $(LIBFT_NAME) $(NAME)

$(LIBFT_NAME):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT_DIR)/$(LIBFT_NAME) -o $(NAME)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(MAKE) clean -C $(LIBFT_DIR)
	rm -f $(OBJS)

fclean: clean
	$(MAKE) fclean -C $(LIBFT_DIR)
	rm -f $(NAME)

re: fclean all

norm:
	-norminette -R CheckForbiddenSourceHeader $(SRCS)
	-norminette -R CheckDefine $(HEADERS)

.PHONY: all clean fclean re norm

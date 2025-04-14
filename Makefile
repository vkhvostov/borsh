NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
SRCS = src/main.c \
		src/utils.c
OBJS = $(SRCS:.c=.o)
HEADERS = includes/borsh.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

norm:
	-norminette -R CheckForbiddenSourceHeader $(SRCS)
	-norminette -R CheckDefine $(HEADERS)

.PHONY: all clean fclean re norm

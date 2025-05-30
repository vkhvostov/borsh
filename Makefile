NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude -I/opt/homebrew/opt/readline/include
LIBFT_DIR=libft
LIBFT_NAME=libft.a
LIBFT := $(LIBFT_DIR)/$(LIBFT_NAME)
SRCS = src/main.c \
		src/utils.c \
		src/lexer/lexer.c \
		src/lexer/pipe_and_redirection.c \
		src/lexer/quotes_and_words.c \
		src/lexer/token_utils.c \
		src/lexer/variable_handler.c \
		src/lexer/variable_utils.c \
		src/execution/process_launcher.c \
		src/execution/command_execution.c \
		src/execution/piping.c \
		src/execution/redirections.c \
		src/execution/heredoc.c \
		src/execution/execute.c
OBJS = $(SRCS:.c=.o)
HEADERS = include/borsh.h # Corrected path

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) -L/opt/homebrew/opt/readline/lib -lreadline $(OBJS) $(LIBFT) -o $(NAME)

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

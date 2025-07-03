NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude -I/opt/homebrew/opt/readline/include
LIBFT_DIR=libft
LIBFT_NAME=libft.a
LIBFT := $(LIBFT_DIR)/$(LIBFT_NAME)
SRCS = src/main.c \
		src/utils.c \
		src/env_utils.c \
		src/lexer/lexer.c \
		src/lexer/pipe_and_redirection.c \
		src/lexer/quotes_and_words.c \
		src/lexer/quote_handling.c \
		src/lexer/quote_content.c \
		src/lexer/word_handling.c \
		src/lexer/token_creation.c \
		src/lexer/token_utils.c \
		src/lexer/token_utils2.c \
		src/lexer/variable_handler.c \
		src/lexer/variable_expansion.c \
		src/lexer/variable_utils.c \
		src/parser/parser_utils.c \
		src/parser/parser_freeing_func.c \
		src/parser/tokens_handlers.c \
		src/parser/parser.c \
		src/test_utils.c \
		src/execution/execution_buildin.c \
		src/execution/execution_commands.c \
		src/execution/execution_commands2.c \
		src/execution/execution_signals.c \
		src/execution/execution_utils.c \
		src/execution/execution_utils2.c \
		src/execution/execution.c \
		src/execution/process_handling.c \
		src/execution/path_resolution.c \
		src/execution/signal_handling.c \
		src/execution/redirection_utils.c \
		src/execution/redirections.c \
		src/execution/heredoc.c \
		src/builtins/builtins.c \
		src/builtins/echo.c \
		src/builtins/execute_builtin.c \
		src/builtins/cd.c \
		src/builtins/builtin_utils.c \
		src/builtins/pwd.c \
		src/builtins/export.c \
		src/builtins/env.c \
		src/builtins/unset.c \
		src/builtins/exit.c


OBJS = $(SRCS:.c=.o)
HEADERS = include/borsh.h

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

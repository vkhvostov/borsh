#ifndef BORSH_H
# define BORSH_H

# include "../libft/libft.h"

// printf, perror
# include <stdio.h>
// malloc, free, exit
# include <stdlib.h>
// access, read, write, close, fork, execve, getcwd, chdir, dup, dup2, isatty
// ttyname, ttyslot
# include <unistd.h>
// open, O_* flags
# include <fcntl.h>
// strerror
# include <string.h>
// stat, lstat, fstat
# include <sys/stat.h>
// wait, waitpid, wait3, wait4, stat, lstat, fstat, opendir, readdir
# include <sys/types.h>
// wait, waitpid, wait3, wait4
# include <sys/wait.h>
// opendir, readdir, closedir
# include <dirent.h>
// signal, sigaction, sigemptyset, sigaddset, kill
# include <signal.h>
// tcsetattr, tcgetattr
# include <termios.h>
// tgetent (sometimes required by curses), might be optional
# include <curses.h>
// tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
# include <term.h>
// ioctl
# include <sys/ioctl.h>
// readline, rl_clear_history, rl_on_new_line, rl_replace_line, rl_redisplay
# include <readline/readline.h>
// add_history
# include <readline/history.h>
// errno (for error handling if needed)
# include <errno.h>

typedef enum	e_token_type {
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_HEREDOC
}	t_token_type;

typedef struct	s_token {
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

// lexer
t_token	*parse_pipe(int *i);
t_token	*parse_redirection(char *input, int *i);
t_token	*parse_single_quote(char *input, int *i);
t_token	*parse_double_quote(char *input, int *i);
t_token	*parse_word(char *input, int *i);
t_token	*lexer(char *input);
void	add_token(t_token **token_list, t_token *new_token);
void	free_tokens(t_token *token_list);
void	token_handler(char *input, t_token **current_token, int *i);

// utils
int		is_word_char(char c);
void	skip_whitespace(char *input, int *i);


#endif

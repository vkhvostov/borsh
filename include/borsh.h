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

// TODO: verify that this is allowed
extern int	g_exit_status;

// lexer
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

// parser
typedef enum e_redirect_type {
	// <
	REDIR_IN,
	// >
	REDIR_OUT,
	// >>
	REDIR_APPEND,
	// <<
	REDIR_HEREDOC
}	t_redirect_type;

typedef struct s_redirect {
	t_redirect_type		type;
	char				*file;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_command {
	char			*cmd_name;
	char			**argv;
	t_redirect		*in_redir;
	t_redirect		*out_redir;
	struct s_command *next; // для пайпів
}	t_command;

// lexer
t_token	*parse_pipe(int *i);
t_token	*parse_redirection(char *input, int *i);
t_token	*parse_single_quote(char *input, int *i);
t_token	*parse_double_quote(char *input, int *i);
t_token	*parse_word(char *input, int *i);
t_token	*lexer(char *input);
void	add_token(t_token **token_list, t_token *new_token);
void	free_tokens(t_token *token_list);
void	handle_token(char *input, t_token **current_token, int *i);
char	*expand_variables(const char *input);
char	*empty_string(void);
char	*get_variable_value(const char *name);
int		append_chars(const char *input, size_t i, char **result);

// parser
t_command	*parse_tokens(t_token *tokens);
void		free_commands(t_command *cmd);

// utils
int		is_word_char(char c);
void	skip_whitespace(char *input, int *i);

#endif

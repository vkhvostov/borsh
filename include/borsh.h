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

// Global variable to store only the signal number
extern volatile sig_atomic_t g_signal_status;

// Signal handling functions
void	setup_signal_handlers(void);
void	reset_signal_handlers(void);
int		get_last_exit_status(void);
void	set_last_exit_status(int status);

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
typedef struct s_redirect {
	t_token_type		type;
	char				*file;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_command {
	char			*cmd_name;
	char			**argv;
	char			**env;
	t_redirect		*redirs;
	struct			s_command *next;
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
char	*expand_variables(const char *input, char **env);
char	*empty_string(void);
char	*get_variable_value(const char *name, char **env);
int		append_chars(const char *input, size_t i, char **result);

// parser
t_command	*parse_tokens(t_token *tokens, char **env);
void		free_commands(t_command *cmd);
void		handle_pipe_tokens(t_token **tokens, t_command **current, char **env);
t_command	*init_command(char **env);
int			add_arg(char ***argv, char *value);
void		free_argv(char **argv);
void		memory_error_handler(void);

// builtin
int			is_builtin(t_command *cmd);
int			execute_builtin(t_command *cmd, char ***env);
int			builtin_echo(char **argv);
int			builtin_cd(char **argv);
int			builtin_exit(char **argv);
int			builtin_pwd(void);
int			builtin_export(char **argv, char ***env);
int			builtin_unset(char **argv, char ***env);
int			builtin_env(char **argv, char ***env);
int			handle_export_arg(char *arg, char ***env);
int			set_env_var(char ***env, char *var_name, char *value);
int			is_valid_var_name(char *var);

// utils
int		is_word_char(char c);
void	skip_whitespace(char *input, int *i);
int		handle_word_tokens(t_command *current, t_token *tokens);
void	handle_redir_tokens(t_redirect **redir_list, t_token **tokens, 
							t_token_type type);
void	free_shell_env(char **shell_env);
void	hide_ctrl_c_echo(void);
char	**copy_environment(char **system_env);

// debugging
void	print_tokens(t_token *token_list);
void	print_redirects(t_redirect *redir_list, const char *label);
void	print_commands(t_command *cmd_list);

void	execute(t_command *commands, char ***env);

#endif

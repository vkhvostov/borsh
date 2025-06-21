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
extern volatile sig_atomic_t	g_signal_status;

// Signal handling functions
void		setup_signal_handlers(int *exit_status);
void		reset_signal_handlers(int *exit_status);

// Exit status functions
// int			get_last_exit_status(void);
// void		set_last_exit_status(int status);

// lexer
typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_REDIR_IN,
	T_REDIR_OUT,
	T_REDIR_APPEND,
	T_HEREDOC
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

// parser
typedef struct s_redirect
{
	t_token_type		type;
	char				*file;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_command
{
	char				*cmd_name;
	char				**argv;
	char				**env;
	t_redirect			*redirs;
	struct s_command	*next;
}	t_command;

/* typedef struct s_var_ctx
{
	char	*input;
	size_t	*i;
	char	**result;
	char	**env;
}	t_var_ctx; */

// lexer
t_token		*parse_pipe(int *i);
t_token		*parse_redirection(char *input, int *i);
t_token		*parse_single_quote(char *input, int *i, int *exit_status);
t_token		*parse_double_quote(char *input, int *i, int *exit_status);
t_token		*parse_word(char *input, int *i, int *exit_status);
t_token		*lexer(char *input, int *exit_status);
void		add_token(t_token **token_list, t_token *new_token);
void		free_tokens(t_token *token_list);
void		handle_token(char *input, t_token **current_token, int *i,
				int *exit_status);
char		*expand_variables(const char *input, char **env, int *exit_status);
char		*empty_string(void);
char		*get_variable_value(const char *name, char **env, int *exit_status);
int			append_chars(const char *input, size_t i, char **result);
int			process_expansion(const char *input, size_t *i,
				char **result, char **env, int *exit_status);

// quote handling
// int			handle_unclosed_quote(char quote_type);
int			parse_quoted_part_loop(char *input, int *i, char quote_type,
				int *exit_status);
char		*handle_quoted_content(char *input, int quote_start, int quote_end);
char		*handle_quoted_part(char *input, int *i, char *result, 
				int *exit_status);
char		*handle_quoted_part_result(char *result, char *quoted);
char		*handle_single_quote_content(char *input, int start,
				int quote_start, int quote_end);
char		*handle_single_quote_after(char *input, int *i,
				int quote_end, char *result);
char		*handle_double_quote_content(char *input, int *i, char *result,
				int *exit_status);

// word handling
char		*handle_word_content(char *input, int start, int end);
char		*handle_word_part(char *input, int *i, char *result);
char		*join_word_and_quoted(char *word, char *quoted);
t_token		*handle_word_with_quote(char *input, int *i, char *word, 
				int *exit_status);

// token creation
t_token		*create_word_token(char *value);
t_token		*create_single_quote_token(char *result);

// parser
t_command	*parse_tokens(t_token *tokens, char **env, int *exit_status);
void		free_commands(t_command *cmd);
void		handle_pipe_tokens(t_token **tokens, t_command **current,
				char **env, int *exit_status);
t_command	*init_command(char **env, int *exit_status);
int			add_arg(char ***argv, char *value, int *exit_status);
void		free_argv(char **argv);
void		memory_error_handler(int *exit_status);
int			handle_word_tokens(t_command *current, t_token *tokens, 
				int *exit_status);
void		handle_redir_tokens(t_redirect **redir_list, t_token **tokens,
				t_token_type type, int *exit_status);

// builtin
int			is_builtin(t_command *cmd);
int			execute_builtin(t_command *cmd, char ***env, int *exit_status);
int			builtin_echo(char **argv);
int			builtin_cd(char **argv);
int			builtin_exit(char **argv, int *exit_status);
int			builtin_pwd(void);
int			builtin_export(char **argv, char ***env);
int			builtin_unset(char **argv, char ***env);
int			builtin_env(char **argv, char ***env);
int			handle_export_arg(char *arg, char ***env);
int			set_env_var(char ***env, char *var_name, char *value);
int			is_valid_var_name(char *var);

// utils
int			is_word_char(char c);
void		skip_whitespace(char *input, int *i);
void		free_shell_env(char **shell_env);
void		hide_ctrl_c_echo(void);

// env utils
char		**copy_environment(char **system_env, int *exit_status);

// debugging
void		print_tokens(t_token *token_list);
void		print_redirects(t_redirect *redir_list, const char *label);
void		print_commands(t_command *cmd_list);

// execution
typedef struct s_process_params
{
	int		in_fd;
	int		out_fd;
	int		pipe_fds[2];
	bool	is_last_command;
	char	***env;
}	t_process_params;

typedef struct s_io_ctx
{
	int		*fds;
	int		*pipe_fds;
	bool	is_last;
}	t_io_ctx;

typedef struct s_cmd_ctx
{
	t_command	*cmd;
	pid_t		*pids;
	int			cmd_idx;
	int			*prev_pipe_read;
	int			fds[2];
	int			pipe_fds[2];
	bool		is_last;
	char		***env;
}	t_cmd_ctx;

void		execute(t_command *commands, char ***env, int *exit_status);

pid_t		launch_process(t_command *command, t_process_params params, 
				int *exit_status);
char		*resolve_path(char *command_name, int *exit_status);
int			handle_redirections(t_command *command, int *in_fd, int *out_fd,
				int *exit_status);
int			handle_heredoc(t_redirect *redir, int *heredoc_pipe_fd, 
				int *exit_status);
void		close_pipe_fds(int *pipe_fds);
void		handle_exec_error(t_command *command);
int			count_commands(t_command *commands);
void		safe_close(int fd);
void		cleanup_command_resources(int *fds, int *pipe_fds);
void		wait_for_children(pid_t *pids, int cmd_idx, int *exit_status);
void		handle_builtin_command(t_cmd_ctx *ctx, int *exit_status);
void		setup_command_io(t_cmd_ctx *ctx, bool *should_skip_command, 
				int *exit_status);
void		handle_skipped_command(t_cmd_ctx *ctx, int *exit_status);
void		handle_command_resolution(t_cmd_ctx *ctx, char *original,
				int *exit_status);
void		prepare_process_params(t_cmd_ctx *ctx, t_process_params *params);
void		close_used_fds(t_cmd_ctx *ctx);
void		process_command(t_cmd_ctx *ctx, int *exit_status);

void		print_error_with_file(char *file, char *error);
void		close_fd_safe(int *fd);
int			open_file_with_flags(char *file, int flags, int *exit_status);
int			handle_redirection_error(int *in_fd, int *out_fd);
int			get_output_flags(t_redirect *redir, int *exit_status);

#endif

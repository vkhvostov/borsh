#ifndef EXECUTION_H
# define EXECUTION_H

# include "borsh.h"

void execute(t_command *commands);

pid_t launch_process(t_command *command, t_process_params params);
char *resolve_path(char *command_name);
int handle_redirections(t_command *command, int *in_fd, int *out_fd);
int handle_heredoc(t_redirect *redir, int *heredoc_pipe_fd);

#endif

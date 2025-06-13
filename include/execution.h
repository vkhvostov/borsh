#ifndef EXECUTION_H
# define EXECUTION_H

# include "borsh.h"

typedef struct s_process_params
{
    int     in_fd;
    int     out_fd;
    int     pipe_fds[2];
    bool    is_last_command;
}   t_process_params;

void execute(t_command *commands, char ***env);

// Anticipated helper function prototypes
pid_t launch_process(t_command *command, t_process_params params);
char *resolve_path(char *command_name);
int handle_redirections(t_command *command, int *in_fd, int *out_fd);
int handle_heredoc(t_redirect *redir, int *heredoc_pipe_fd);

#endif

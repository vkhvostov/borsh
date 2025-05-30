#ifndef EXECUTION_H
# define EXECUTION_H

# include "../../include/borsh.h" // Assuming borsh.h is in root/include

// Forward declaration if t_command is defined elsewhere, e.g., in borsh.h
// If not, you might need to include the definition of t_command before this.
// For now, assuming t_command is known from borsh.h or another included header.

void execute(t_command *commands);

// Anticipated helper function prototypes
// void launch_process(t_command *command, int in_fd, int out_fd);
// char *resolve_path(char *command_name);
// int handle_redirections(t_command *command, int *in_fd, int *out_fd);
// int handle_heredoc(t_redirect *redir);
// void wait_for_processes(pid_t *pids, int count);
// void setup_pipes(t_command *commands, int num_commands, int (*pipes)[2]);
// void close_pipes(int num_pipes, int (*pipes)[2]);

#endif // EXECUTION_H

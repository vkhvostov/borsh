#include "../../include/execution.h"

pid_t launch_process(t_command *command, int in_fd, int out_fd, int pipe_fds[2], bool is_last_command) {
	pid_t pid = fork();

	if (pid == -1) {
		perror("fork failed");
		return -1;
	}

	if (pid == 0) { // Child process
		// Redirect input if necessary
		if (in_fd != STDIN_FILENO) {
			if (dup2(in_fd, STDIN_FILENO) == -1) {
				perror("dup2 in_fd failed");
				exit(EXIT_FAILURE);
			}
			close(in_fd); // Close original after duplication
		}

		// Redirect output if necessary
		// This handles output to a file or the initial setup for piping.
		if (out_fd != STDOUT_FILENO) {
			if (dup2(out_fd, STDOUT_FILENO) == -1) {
				perror("dup2 out_fd failed");
				exit(EXIT_FAILURE);
			}
			close(out_fd); // Close original after duplication
		}

		// If there's a pipe and this command is not the last one,
		// it should write to the pipe.
		// The out_fd might have been set to pipe_fds[1] by the caller,
		// or we explicitly handle it here.
		// The current logic assumes out_fd would be pre-set to pipe_fds[1] if needed.
		// Let's refine based on typical pipe usage:
		if (pipe_fds != NULL) {
			if (!is_last_command) { // This process writes to the pipe
				if (out_fd != pipe_fds[1]) { // If out_fd wasn't already the pipe's write end
					if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
						perror("dup2 pipe_fds[1] failed");
						exit(EXIT_FAILURE);
					}
				}
			}
			// Child closes both ends of the pipe it's directly involved with
			// or that were passed to it.
			close(pipe_fds[0]); // Close read end in child
			close(pipe_fds[1]); // Close write end in child (it's now STDOUT_FILENO or closed)
		}

		// Execute the command
		if (command->cmd_name == NULL || command->argv == NULL) {
			fprintf(stderr, "Error: command or arguments are NULL.\n");
			exit(EXIT_FAILURE);
		}
		
		execve(command->cmd_name, command->argv, command->env);
		
		// If execve returns, it's an error
		perror("execve failed");
		exit(EXIT_FAILURE);
	}

	return pid;
}

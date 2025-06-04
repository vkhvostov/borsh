#include "../../include/execution.h"


// Helper to count commands
static int count_commands(t_command *commands) {
	int count = 0;
	t_command *current = commands;
	while (current != NULL) {
		count++;
		current = current->next;
	}
	return count;
}

// Helper to close FDs, ensuring not to close standard ones unless intended
static void safe_close(int fd) {
	if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO && fd != -1) {
		close(fd);
	}
}

void execute(t_command *commands) {
	if (commands == NULL) {
		return;
	}

	int num_cmds = count_commands(commands);
	if (num_cmds == 0) {
		return;
	}

	pid_t *pids = malloc(num_cmds * sizeof(pid_t));
	if (pids == NULL) {
		perror("malloc for pids failed");
		g_exit_status = 1; // General error
		return;
	}
	for(int i=0; i<num_cmds; ++i) pids[i] = -1; // Initialize pids

	t_command *current_cmd = commands;
	int cmd_idx = 0;
	int prev_pipe_read_end = -1; // Read end of the pipe from the previous command

	while (current_cmd != NULL) {
		int fds_for_current_cmd[2] = {STDIN_FILENO, STDOUT_FILENO}; // [0] is in_fd, [1] is out_fd
		bool is_last_command = (current_cmd->next == NULL);
		int next_pipe_fds[2] = {-1, -1}; // Pipe for current_cmd to next_cmd

		// 1. Setup pipe for output if current_cmd is not the last one
		if (!is_last_command) {
			if (pipe(next_pipe_fds) == -1) {
				perror("pipe failed");
				g_exit_status = 1;
				safe_close(prev_pipe_read_end);
				break;
			}
		}

		// 2. Handle input setup
		if (prev_pipe_read_end != -1) {
			// If we have pipe input, use it
			fds_for_current_cmd[0] = prev_pipe_read_end;
		} else {
			// No pipe input, check for input redirections
			if (handle_redirections(current_cmd, &fds_for_current_cmd[0], NULL) == -1) {
				g_exit_status = 1;
				if (!is_last_command) {
					safe_close(next_pipe_fds[0]);
					safe_close(next_pipe_fds[1]);
				}
				current_cmd = current_cmd->next;
				cmd_idx++;
				continue;
			}
		}

		// 3. Handle output setup
		if (!is_last_command) {
			// If we have a next command, use pipe
			fds_for_current_cmd[1] = next_pipe_fds[1];
		} else {
			// Last command, check for output redirections
			if (handle_redirections(current_cmd, NULL, &fds_for_current_cmd[1]) == -1) {
				g_exit_status = 1;
				safe_close(fds_for_current_cmd[0]);
				if (!is_last_command) {
					safe_close(next_pipe_fds[0]);
					safe_close(next_pipe_fds[1]);
				}
				current_cmd = current_cmd->next;
				cmd_idx++;
				continue;
			}
		}

		// 4. Resolve Command Path
		char *original_cmd_name = current_cmd->cmd_name;
		if (current_cmd->cmd_name != NULL && current_cmd->cmd_name[0] != '\0') {
			current_cmd->cmd_name = resolve_path(current_cmd->cmd_name);
		}

		if (current_cmd->cmd_name == NULL && original_cmd_name != NULL && original_cmd_name[0] != '\0') {
			fprintf(stderr, "borsh: command not found: %s\n", original_cmd_name);
			g_exit_status = 127;
			current_cmd->cmd_name = original_cmd_name;
			safe_close(fds_for_current_cmd[0]);
			safe_close(fds_for_current_cmd[1]);
			if (!is_last_command) {
				safe_close(next_pipe_fds[0]);
			}
			current_cmd = current_cmd->next;
			cmd_idx++;
			continue;
		}

		// 5. Launch Process
		pids[cmd_idx] = launch_process(current_cmd, fds_for_current_cmd[0], fds_for_current_cmd[1], 
									next_pipe_fds, is_last_command);

		// Restore and free path, if it was resolved
		if (current_cmd->cmd_name != original_cmd_name) {
			free(current_cmd->cmd_name);
			current_cmd->cmd_name = original_cmd_name;
		}

		if (pids[cmd_idx] == -1) {
			g_exit_status = 1;
			safe_close(fds_for_current_cmd[0]);
			safe_close(fds_for_current_cmd[1]);
			if (!is_last_command) {
				safe_close(next_pipe_fds[0]);
			}
			break;
		}

		// 6. Parent Process Post-Launch Cleanup
		safe_close(fds_for_current_cmd[0]); // Close input (could be prev_pipe_read_end)
		safe_close(fds_for_current_cmd[1]); // Close output (could be next_pipe_fds[1])

		// Setup for next command
		prev_pipe_read_end = is_last_command ? -1 : next_pipe_fds[0];
		current_cmd = current_cmd->next;
		cmd_idx++;
	}

	// Ensure any remaining prev_pipe_read_end is closed
	safe_close(prev_pipe_read_end);

	// 7. Wait for all successfully launched child processes
	for (int i = 0; i < cmd_idx; i++) {
		if (pids[i] > 0) {
			int status;
			if (waitpid(pids[i], &status, 0) == -1) {
				perror("waitpid failed");
				if (i == num_cmds - 1) {
					g_exit_status = 1;
				}
				continue;
			}
			if (WIFEXITED(status)) {
				if (i == num_cmds - 1 || cmd_idx == 1) {
					g_exit_status = WEXITSTATUS(status);
				}
			} else if (WIFSIGNALED(status)) {
				g_exit_status = 128 + WTERMSIG(status);
			}
		}
	}

	free(pids);
}

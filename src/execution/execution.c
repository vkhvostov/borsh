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

		// 1. Handle Redirections for the current command
		if (handle_redirections(current_cmd, &fds_for_current_cmd[0], &fds_for_current_cmd[1]) == -1) {
			g_exit_status = 1; // Redirection error
			// FDs managed by handle_redirections on its error.
			// prev_pipe_read_end might need closing if it exists from a prior successful command
			safe_close(prev_pipe_read_end);
			prev_pipe_read_end = -1; 
			// Skip this command and try next one? Or abort all?
			// For now, skip this command.
			current_cmd = current_cmd->next;
			cmd_idx++; // Increment to keep pids array in sync, though this pid will be -1
			continue;
		}

		// 2. Setup Input FD for launch_process
		// If there's a pipe from the previous command, it's the input.
		if (prev_pipe_read_end != -1) {
			safe_close(fds_for_current_cmd[0]); // Close file redirection if any, pipe takes precedence
			fds_for_current_cmd[0] = prev_pipe_read_end;
		}

		// 3. Setup Pipe for output if current_cmd is not the last one
		if (!is_last_command) {
			if (pipe(next_pipe_fds) == -1) {
				perror("pipe failed");
				g_exit_status = 1;
				safe_close(fds_for_current_cmd[0]);
				safe_close(fds_for_current_cmd[1]);
				// Close any previously created prev_pipe_read_end from *this* execution sequence
				// prev_pipe_read_end is already handled by its own variable management
				// This is a fatal error for the pipeline.
				break; 
			}
			safe_close(fds_for_current_cmd[1]); // Close file redirection if any, pipe takes precedence
			fds_for_current_cmd[1] = next_pipe_fds[1]; // Output of current goes to write end of new pipe
		}

		// 4. Resolve Command Path
		// Handle builtins separately if not done by resolve_path/launch_process
		// For now, assume all are external commands or builtins that can be forked.
		char *original_cmd_name = current_cmd->cmd_name; // For restoration
		if (current_cmd->cmd_name != NULL && current_cmd->cmd_name[0] != '\0') { // only resolve if not empty
			current_cmd->cmd_name = resolve_path(current_cmd->cmd_name);
		} // else cmd_name is NULL or empty, launch_process should handle.
	

		if (current_cmd->cmd_name == NULL && original_cmd_name != NULL && original_cmd_name[0] != '\0') {
			fprintf(stderr, "borsh: command not found: %s\n", original_cmd_name);
			g_exit_status = 127;
			current_cmd->cmd_name = original_cmd_name; // Restore for safety/consistency
			// Cleanup FDs for this failed command
			safe_close(fds_for_current_cmd[0]);
			safe_close(fds_for_current_cmd[1]); // This would be next_pipe_fds[1] if pipe was created
			if (!is_last_command && next_pipe_fds[0] != -1) { // if pipe was created
				safe_close(next_pipe_fds[0]); // close read end of new pipe too
			}
			prev_pipe_read_end = -1; // Current prev_pipe_read_end was used or closed.
									// next_pipe_fds[0] won't become prev_pipe_read_end.
			current_cmd = current_cmd->next;
			cmd_idx++;
			continue;
		}
		
		// 5. Launch Process
		pids[cmd_idx] = launch_process(current_cmd, fds_for_current_cmd[0], fds_for_current_cmd[1], next_pipe_fds, is_last_command);

		// Restore and free path, if it was resolved
		if (current_cmd->cmd_name != original_cmd_name) { // Path was resolved
			free(current_cmd->cmd_name); // Free the strdup'd path from resolve_path
			current_cmd->cmd_name = original_cmd_name;
		}

		if (pids[cmd_idx] == -1) { // Fork failed
			g_exit_status = 1; // General error
			// FDs for this command are typically handled by launch_process parent side on fork failure for its direct inputs
			// but pipe specific FDs for *this* stage need care.
			safe_close(fds_for_current_cmd[0]); // Close input (could be prev_pipe_read_end)
			safe_close(fds_for_current_cmd[1]); // Close output (could be next_pipe_fds[1])
			if (!is_last_command && next_pipe_fds[0] != -1) { // if pipe was created
				safe_close(next_pipe_fds[0]); // close read end of new pipe
			}
			prev_pipe_read_end = -1; // Reset pipe chain
			// Attempt to continue or break? For now, break on fork failure.
			break;
		}

		// 6. Parent Process Post-Launch Cleanup for this command's FDs
		// Input FD for current command (file or prev_pipe_read_end) was duplicated by child or is STDIN.
		// Parent should close its copy if it's not STDIN. launch_process might do some of this.
		// Specifically, prev_pipe_read_end needs to be closed here by the main loop.
		safe_close(fds_for_current_cmd[0]); // This closes prev_pipe_read_end if it was used.
		prev_pipe_read_end = -1; // Mark as consumed or closed.

		// Output FD for current command (file or next_pipe_fds[1]) was duplicated by child or is STDOUT.
		// Parent should close its copy of the write end of the pipe.
		safe_close(fds_for_current_cmd[1]); // This closes next_pipe_fds[1] if it was used.

		// If there was a pipe to the next command, its read end becomes prev_pipe_read_end for the next iteration.
		if (!is_last_command && next_pipe_fds[0] != -1) {
			prev_pipe_read_end = next_pipe_fds[0]; // Save read end for the next command
		} else {
			prev_pipe_read_end = -1; // Ensure it's reset if no pipe or last command
		}
		
		current_cmd = current_cmd->next;
		cmd_idx++;
	}
	
	// If loop broke early due to critical error (pipe fail, fork fail), cmd_idx reflects commands processed up to error.
	// Ensure any remaining prev_pipe_read_end is closed if loop exited prematurely
	safe_close(prev_pipe_read_end);

	// 7. Wait for all successfully launched child processes
	for (int i = 0; i < cmd_idx; i++) {
		if (pids[i] > 0) { // If launch was attempted and PID is valid
			int status;
			if (waitpid(pids[i], &status, 0) == -1) {
				perror("waitpid failed");
				// Set g_exit_status? Or rely on status of other commands?
				// If one waitpid fails, it's a problem with this process, but others might be fine.
				// Usually, the shell's exit status is that of the last command.
				// For now, let's assume if waitpid fails, we can't get its status.
				if (i == num_cmds -1 ) { // if it was the last command that failed at waitpid
					g_exit_status = 1; // general error
				}
				continue;
			}
			if (WIFEXITED(status)) {
				if (i == num_cmds - 1 || cmd_idx == 1) { // If it's the last command of the pipe, or only command
					g_exit_status = WEXITSTATUS(status);
				}
			} else if (WIFSIGNALED(status)) {
				// Handle signaled termination
				// E.g. Ctrl+C (SIGINT) makes command exit with 130
				// Standard bash practice: 128 + signal number
				g_exit_status = 128 + WTERMSIG(status);
				// Optionally print signal message to stderr, e.g. "Terminated by signal X"
				// Check if it's the last command to set global status
				// if (i == cmd_idx - 1) { // If this signaled process was the last one.
				//    // Handled by setting g_exit_status directly.
				// }
				// If a command is killed by a signal, subsequent commands in pipeline usually don't run.
				// Current loop structure will try to run them unless a 'break' was hit.
				// This part may need refinement based on precise signal handling requirements.
			}
		} else {
			// If pids[i] is -1, it means this command failed before or during launch.
			// g_exit_status should have been set at the point of failure for that command.
			// The overall g_exit_status will typically be that of the last command *executed*.
			// If a command like `false | true` is run, exit status is 0.
			// If `nonexistent | true` is run, exit status is 0 (if `true` runs).
			// If `true | nonexistent` is run, exit status is 127.
			// The logic for g_exit_status based on the last command in the pipe is key.
			// The current WIFEXITED block handles this for the last command.
		}
	}

	free(pids);
}

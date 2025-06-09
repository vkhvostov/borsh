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

static void cleanup_command_resources(int *fds, int *pipe_fds)
{
	safe_close(fds[0]);
	safe_close(fds[1]);
	if (pipe_fds)
	{
		safe_close(pipe_fds[0]);
		safe_close(pipe_fds[1]);
	}
}

static void wait_for_children(pid_t *pids, int cmd_idx)
{
	int i;
	int status;
	int last_cmd_idx = cmd_idx - 1;

	for (i = 0; i < cmd_idx; i++)
	{
		if (pids[i] > 0)
		{
			if (waitpid(pids[i], &status, 0) == -1)
			{
				perror("waitpid failed");
				if (i == last_cmd_idx)
					set_last_exit_status(1);
				continue;
			}
			// Only set exit status for the last command in the pipeline
			if (i == last_cmd_idx)
			{
				if (WIFEXITED(status))
					set_last_exit_status(WEXITSTATUS(status));
				else if (WIFSIGNALED(status))
				{
					if (WTERMSIG(status) == SIGINT)
					{
						set_last_exit_status(130);  // 128 + SIGINT
						write(STDERR_FILENO, "\n", 1);
					}
					else if (WTERMSIG(status) == SIGQUIT)
					{
						set_last_exit_status(131);  // 128 + SIGQUIT
						write(STDERR_FILENO, "Quit: 3\n", 8);
					}
					else
						set_last_exit_status(128 + WTERMSIG(status));
				}
			}
		}
	}
}

static void setup_command_io(t_command *cmd, int *fds, int *pipe_fds, bool is_last, bool *should_skip_command)
{
	fds[0] = STDIN_FILENO;
	fds[1] = STDOUT_FILENO;
	*should_skip_command = false;
	
	if (!is_last && pipe(pipe_fds) == -1)
	{
		perror("pipe failed");
		set_last_exit_status(1);  // Pipe error
		*should_skip_command = true;
		return;
	}
	
	if (handle_redirections(cmd, &fds[0], &fds[1]) == -1)
	{
		set_last_exit_status(1);  // Redirection error
		*should_skip_command = true;
		if (!is_last)
		{
			safe_close(pipe_fds[0]);
			safe_close(pipe_fds[1]);
		}
	}
}

static void process_command(t_command *cmd, pid_t *pids, int cmd_idx, 
						  int *prev_pipe_read, int *fds, int *pipe_fds, bool is_last)
{
	char *original_cmd_name;
	t_process_params params;
	bool should_skip_command;

	setup_command_io(cmd, fds, pipe_fds, is_last, &should_skip_command);
	if (should_skip_command)
	{
		// Even if we skip the command, we need to set up pipes for the next command
		if (!is_last)
		{
			close(pipe_fds[1]);  // Close write end
			*prev_pipe_read = pipe_fds[0];  // Save read end for next command
		}
		return;
	}

	// Set up input from previous pipe if it exists
	if (*prev_pipe_read != -1)
	{
		fds[0] = *prev_pipe_read;
		*prev_pipe_read = -1;  // Will be closed by launch_process
	}

	original_cmd_name = cmd->cmd_name;
	if (cmd->cmd_name != NULL && cmd->cmd_name[0] != '\0')
		cmd->cmd_name = resolve_path(cmd->cmd_name);

	if (cmd->cmd_name == NULL && original_cmd_name != NULL && original_cmd_name[0] != '\0')
	{
		fprintf(stderr, "borsh: command not found: %s\n", original_cmd_name);
		set_last_exit_status(127);  // Command not found
		cmd->cmd_name = original_cmd_name;
		cleanup_command_resources(fds, !is_last ? pipe_fds : NULL);
		return;
	}

	params.in_fd = fds[0];
	params.out_fd = fds[1];
	if (!is_last)
	{
		params.pipe_fds[0] = pipe_fds[0];
		params.pipe_fds[1] = pipe_fds[1];
	}
	else
	{
		params.pipe_fds[0] = -1;
		params.pipe_fds[1] = -1;
	}
	params.is_last_command = is_last;

	pids[cmd_idx] = launch_process(cmd, params);

	if (cmd->cmd_name != original_cmd_name)
	{
		free(cmd->cmd_name);
		cmd->cmd_name = original_cmd_name;
	}

	if (pids[cmd_idx] == -1)
	{
		set_last_exit_status(1);  // Process launch error
		cleanup_command_resources(fds, !is_last ? pipe_fds : NULL);
		return;
	}

	// Close input FD if it was from a pipe
	if (fds[0] != STDIN_FILENO)
		close(fds[0]);
	
	// Close output FD if it was redirected
	if (fds[1] != STDOUT_FILENO)
		close(fds[1]);

	// Set up pipe for next command
	if (!is_last)
	{
		close(pipe_fds[1]);  // Close write end, we don't need it anymore
		*prev_pipe_read = pipe_fds[0];  // Save read end for next command
	}
}

void execute(t_command *commands)
{
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
		set_last_exit_status(1);  // Memory error
		return;
	}
	for(int i=0; i<num_cmds; ++i) pids[i] = -1; // Initialize pids

	t_command *current_cmd = commands;
	int cmd_idx = 0;
	int prev_pipe_read = -1; // Read end of the pipe from the previous command
	int fds[2];
	int pipe_fds[2];

	while (current_cmd != NULL) {
		process_command(current_cmd, pids, cmd_idx, &prev_pipe_read, 
					   fds, pipe_fds, current_cmd->next == NULL);
		current_cmd = current_cmd->next;
		cmd_idx++;
	}

	// Ensure any remaining prev_pipe_read is closed
	safe_close(prev_pipe_read);

	// Wait for all successfully launched child processes
	wait_for_children(pids, cmd_idx);

	free(pids);
}

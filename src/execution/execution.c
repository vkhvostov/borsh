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
						  int *prev_pipe_read, int *fds, int *pipe_fds, bool is_last, char ***env)
{
	char *original_cmd_name;
	t_process_params params;
	bool should_skip_command;
	bool is_builtin_cmd;
	bool is_single_cmd;
	bool modifies_env;

	setup_command_io(cmd, fds, pipe_fds, is_last, &should_skip_command);
	if (should_skip_command)
	{
		// If we're skipping a command in a pipeline, we need to:
		// 1. Close any previous pipe read end
		if (*prev_pipe_read != -1)
		{
			close(*prev_pipe_read);
			*prev_pipe_read = -1;
		}
		// 2. For non-last commands, create a pipe and immediately close its write end
		if (!is_last)
		{
			if (pipe(pipe_fds) == -1)
			{
				perror("pipe failed");
				set_last_exit_status(1);
				return;
			}
			close(pipe_fds[1]);  // Close write end immediately
			*prev_pipe_read = pipe_fds[0];  // Save read end for next command
		}
		pids[cmd_idx] = -1;  // Mark this command as skipped
		return;
	}

	// Set up input from previous pipe if it exists and no input redirection
	if (*prev_pipe_read != -1 && fds[0] == STDIN_FILENO)
	{
		fds[0] = *prev_pipe_read;
		*prev_pipe_read = -1;  // Will be closed by launch_process
	}

	// Check if it's a builtin command
	is_builtin_cmd = is_builtin(cmd);
	is_single_cmd = (cmd_idx == 0 && cmd->next == NULL);
	modifies_env = is_builtin_cmd && (
		ft_strcmp(cmd->cmd_name, "cd") == 0 ||
		ft_strcmp(cmd->cmd_name, "export") == 0 ||
		ft_strcmp(cmd->cmd_name, "unset") == 0 ||
		ft_strcmp(cmd->cmd_name, "exit") == 0
	);

	// Execute environment-modifying built-ins in parent process
	if (modifies_env && is_single_cmd)
	{
		int status = 0;
		if (ft_strcmp(cmd->cmd_name, "cd") == 0)
			status = builtin_cd(cmd->argv);
		else if (ft_strcmp(cmd->cmd_name, "export") == 0)
			status = builtin_export(cmd->argv, env);
		else if (ft_strcmp(cmd->cmd_name, "unset") == 0)
			status = builtin_unset(cmd->argv, env);
		else if (ft_strcmp(cmd->cmd_name, "exit") == 0)
			status = builtin_exit(cmd->argv);
		set_last_exit_status(status);
		cleanup_command_resources(fds, !is_last ? pipe_fds : NULL);
		return;
	}

	// For builtin commands in pipes or non-env-modifying builtins
	if (is_builtin_cmd)
	{
		pid_t pid = fork();
		if (pid == -1)
		{
			perror("fork failed");
			set_last_exit_status(1);
			return;
		}
		if (pid == 0)
		{
			// Child process
			if (fds[0] != STDIN_FILENO)
				dup2(fds[0], STDIN_FILENO);
			if (fds[1] != STDOUT_FILENO)
				dup2(fds[1], STDOUT_FILENO);
			else if (!is_last)
				dup2(pipe_fds[1], STDOUT_FILENO);

			// Close all pipe FDs
			if (fds[0] != STDIN_FILENO)
				close(fds[0]);
			if (fds[1] != STDOUT_FILENO)
				close(fds[1]);
			if (!is_last)
			{
				close(pipe_fds[0]);
				close(pipe_fds[1]);
			}

			exit(execute_builtin(cmd, env));
		}
		pids[cmd_idx] = pid;

		// Parent process: Close FDs
		if (fds[0] != STDIN_FILENO)
			close(fds[0]);
		if (fds[1] != STDOUT_FILENO)
			close(fds[1]);
		if (!is_last)
		{
			close(pipe_fds[1]);
			*prev_pipe_read = pipe_fds[0];
		}
		return;
	}

	// Handle non-builtin commands
	original_cmd_name = cmd->cmd_name;
	if (cmd->cmd_name != NULL && cmd->cmd_name[0] != '\0')
		cmd->cmd_name = resolve_path(cmd->cmd_name);

	if (cmd->cmd_name == NULL && original_cmd_name != NULL && original_cmd_name[0] != '\0')
	{
		if (errno == EISDIR)
			fprintf(stderr, "borsh: %s: is a directory\n", original_cmd_name);
		else if (errno == EACCES)
			fprintf(stderr, "borsh: %s: Permission denied\n", original_cmd_name);
		else if (errno == ENOENT && strchr(original_cmd_name, '/') != NULL)
			fprintf(stderr, "borsh: %s: No such file or directory\n", original_cmd_name);
		else
			fprintf(stderr, "borsh: %s: command not found\n", original_cmd_name);
		set_last_exit_status(errno == EISDIR || errno == EACCES ? 126 : 127);  // 126 for directory/permission denied, 127 for not found/no such file
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
	params.env = env;
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

void execute(t_command *commands, char ***env)
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
	bool command_executed = false;

	while (current_cmd != NULL) {
		process_command(current_cmd, pids, cmd_idx, &prev_pipe_read, 
					   fds, pipe_fds, current_cmd->next == NULL, env);
		if (pids[cmd_idx] != -1)
			command_executed = true;
		current_cmd = current_cmd->next;
		cmd_idx++;
	}

	// Ensure any remaining prev_pipe_read is closed
	safe_close(prev_pipe_read);

	// Wait for all successfully launched child processes
	if (command_executed)
		wait_for_children(pids, cmd_idx);

	free(pids);
}

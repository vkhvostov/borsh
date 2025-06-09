#include "../../include/execution.h"

static void close_pipe_fds(int *pipe_fds)
{
	if (pipe_fds[0] != -1)
		close(pipe_fds[0]);
	if (pipe_fds[1] != -1)
		close(pipe_fds[1]);
}

static void exit_with_error(const char *msg, int code)
{
	perror(msg);
	exit(code);
}

pid_t launch_process(t_command *command, t_process_params params)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		set_last_exit_status(1);  // Fork error
		return (-1);
	}
	if (pid == 0)
	{
		// Reset signal handlers in child process
		reset_signal_handlers();

		if (params.in_fd != STDIN_FILENO)
		{
			if (dup2(params.in_fd, STDIN_FILENO) == -1)
				exit_with_error("dup2 in_fd failed", 1);
			close(params.in_fd);
		}
		if (params.out_fd != STDOUT_FILENO)
		{
			if (dup2(params.out_fd, STDOUT_FILENO) == -1)
				exit_with_error("dup2 out_fd failed", 1);
			close(params.out_fd);
		}
		if (params.pipe_fds[0] != -1 && params.pipe_fds[1] != -1)
		{
			if (!params.is_last_command)
			{
				if (params.out_fd != params.pipe_fds[1])
				{
					if (dup2(params.pipe_fds[1], STDOUT_FILENO) == -1)
						exit_with_error("dup2 pipe_fds[1] failed", 1);
				}
			}
			close_pipe_fds(params.pipe_fds);
		}
		if (command->cmd_name == NULL || command->argv == NULL)
		{
			fprintf(stderr, "Error: command or arguments are NULL.\n");
			exit(1);
		}
		execve(command->cmd_name, command->argv, command->env);
		// If execve returns, it means it failed
		printf("borsh: %s: %s\n", command->cmd_name, strerror(errno));
		if (errno == ENOENT)
			exit(127);  // Command not found
		else if (errno == EACCES)
			exit(126);  // Command exists but not executable
		exit(1);       // Other error
	}
	else
	{
		// Parent process: Close pipe FDs that are no longer needed
		if (params.pipe_fds[0] != -1 && params.pipe_fds[1] != -1)
		{
			if (!params.is_last_command)
				close(params.pipe_fds[1]); // Close write end in parent for non-last commands
			if (params.in_fd != STDIN_FILENO)
				close(params.in_fd); // Close input FD if it was redirected
		}
	}
	return (pid);
}

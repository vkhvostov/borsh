#include "../../include/execution.h"

static void close_pipe_fds(int *pipe_fds)
{
	if (pipe_fds[0] != -1)
		close(pipe_fds[0]);
	if (pipe_fds[1] != -1)
		close(pipe_fds[1]);
}

pid_t launch_process(t_command *command, t_process_params params)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		return (-1);
	}
	if (pid == 0)
	{
		if (params.in_fd != STDIN_FILENO)
		{
			if (dup2(params.in_fd, STDIN_FILENO) == -1)
			{
				perror("dup2 in_fd failed");
				exit(EXIT_FAILURE);
			}
			close(params.in_fd);
		}
		if (params.out_fd != STDOUT_FILENO)
		{
			if (dup2(params.out_fd, STDOUT_FILENO) == -1)
			{
				perror("dup2 out_fd failed");
				exit(EXIT_FAILURE);
			}
			close(params.out_fd);
		}
		if (params.pipe_fds[0] != -1 && params.pipe_fds[1] != -1)
		{
			if (!params.is_last_command)
			{
				if (params.out_fd != params.pipe_fds[1])
				{
					if (dup2(params.pipe_fds[1], STDOUT_FILENO) == -1)
					{
						perror("dup2 pipe_fds[1] failed");
						exit(EXIT_FAILURE);
					}
				}
			}
			close_pipe_fds(params.pipe_fds);
		}
		if (command->cmd_name == NULL || command->argv == NULL)
		{
			fprintf(stderr, "Error: command or arguments are NULL.\n");
			exit(EXIT_FAILURE);
		}
		execve(command->cmd_name, command->argv, command->env);
		perror("execve failed");
		exit(EXIT_FAILURE);
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

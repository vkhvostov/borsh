#include "../../include/borsh.h"

static int	handle_io_redirection(t_process_params params)
{
	if (params.in_fd != STDIN_FILENO)
	{
		if (dup2(params.in_fd, STDIN_FILENO) == -1)
			return (1);
		close(params.in_fd);
	}
	if (params.out_fd != STDOUT_FILENO)
	{
		if (dup2(params.out_fd, STDOUT_FILENO) == -1)
			return (1);
		close(params.out_fd);
	}
	return (0);
}

static int	handle_pipe_setup(t_process_params params)
{
	if (params.out_fd == STDOUT_FILENO && params.pipe_fds[0] != -1
		&& params.pipe_fds[1] != -1 && !params.is_last_command)
	{
		if (dup2(params.pipe_fds[1], STDOUT_FILENO) == -1)
			return (1);
	}
	if (params.pipe_fds[0] != -1 && params.pipe_fds[1] != -1)
		close_pipe_fds(params.pipe_fds);
	return (0);
}

static int	handle_child_process(t_command *command, t_process_params params)
{
	int	status;

	status = 0;
	if (command->cmd_name == NULL || command->argv == NULL)
	{
		ft_putstr_fd("Error: command or arguments are NULL.\n", 2);
		return (1);
	}
	status = handle_io_redirection(params);
	if (status != 0)
		return (status);
	status = handle_pipe_setup(params);
	if (status != 0)
		return (status);
	if (execve(command->cmd_name, command->argv, command->env) == -1)
		return (handle_exec_error(command));
	return (0);
}

static void	handle_parent_process(t_process_params params)
{
	if (params.pipe_fds[0] != -1 && params.pipe_fds[1] != -1)
	{
		if (!params.is_last_command)
			close(params.pipe_fds[1]);
		if (params.in_fd != STDIN_FILENO)
			close(params.in_fd);
	}
}

pid_t	launch_process(t_command *command, t_process_params params,
	int *exit_status)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("borsh: fork failed\n", 2);
		*exit_status = 1;
		return (-1);
	}
	if (pid == 0)
	{
		reset_signal_handlers(exit_status);
		status = handle_child_process(command, params);
		if (status != 0)
		{
			*exit_status = status;
			return (-1);
		}
	}
	else
		handle_parent_process(params);
	return (pid);
}

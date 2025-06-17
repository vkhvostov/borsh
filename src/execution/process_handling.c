#include "../../include/borsh.h"

static void	handle_io_redirection(t_process_params params)
{
	if (params.in_fd != STDIN_FILENO)
	{
		if (dup2(params.in_fd, STDIN_FILENO) == -1)
			exit(1);
		close(params.in_fd);
	}
	if (params.out_fd != STDOUT_FILENO)
	{
		if (dup2(params.out_fd, STDOUT_FILENO) == -1)
			exit(1);
		close(params.out_fd);
	}
}

static void	handle_pipe_setup(t_process_params params)
{
	if (params.out_fd == STDOUT_FILENO && params.pipe_fds[0] != -1
		&& params.pipe_fds[1] != -1 && !params.is_last_command)
	{
		if (dup2(params.pipe_fds[1], STDOUT_FILENO) == -1)
			exit(1);
	}
	if (params.pipe_fds[0] != -1 && params.pipe_fds[1] != -1)
		close_pipe_fds(params.pipe_fds);
}

static void	handle_child_process(t_command *command, t_process_params params)
{
	if (command->cmd_name == NULL || command->argv == NULL)
	{
		ft_putstr_fd("Error: command or arguments are NULL.\n", 2);
		exit(1);
	}
	handle_io_redirection(params);
	handle_pipe_setup(params);
	execve(command->cmd_name, command->argv, command->env);
	handle_exec_error(command);
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

pid_t	launch_process(t_command *command, t_process_params params)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		ft_putstr_fd("borsh: fork failed\n", 2);
		set_last_exit_status(1);
		return (-1);
	}
	if (pid == 0)
	{
		reset_signal_handlers();
		handle_child_process(command, params);
	}
	else
		handle_parent_process(params);
	return (pid);
}

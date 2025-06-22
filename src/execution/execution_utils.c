#include "../../include/borsh.h"

void	close_pipe_fds(int *pipe_fds)
{
	if (pipe_fds[0] != -1)
		close(pipe_fds[0]);
	if (pipe_fds[1] != -1)
		close(pipe_fds[1]);
}

void	handle_exec_error(t_command *command)
{
	if (errno == EISDIR)
		ft_putstr_fd("borsh: Is a directory\n", 2);
	else
	{
		ft_putstr_fd("borsh: ", 2);
		ft_putstr_fd(command->cmd_name, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
	}
	if (errno == ENOENT)
		exit(127);
	else if (errno == EACCES || errno == EISDIR)
		exit(126);
	exit(1);
}

void	safe_close(int fd)
{
	if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO
		&& fd != -1)
		close(fd);
}

int	count_commands(t_command *commands)
{
	int			count;
	t_command	*current;

	count = 0;
	current = commands;
	while (current != NULL)
	{
		count++;
		current = current->next;
	}
	return (count);
}

void	cleanup_command_resources(int *fds, int *pipe_fds)
{
	safe_close(fds[0]);
	safe_close(fds[1]);
	if (pipe_fds)
	{
		safe_close(pipe_fds[0]);
		safe_close(pipe_fds[1]);
	}
}

#include "../../include/borsh.h"

void	print_error_with_file(char *file, char *error)
{
	ft_putstr_fd("borsh: ", 2);
	ft_putstr_fd(file, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(error, 2);
	ft_putstr_fd("\n", 2);
}

void	close_fd_safe(int *fd)
{
	if (*fd != STDIN_FILENO && *fd != STDOUT_FILENO)
		close(*fd);
}

int	open_file_with_flags(char *file, int flags)
{
	int	fd;

	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		print_error_with_file(file, strerror(errno));
		set_last_exit_status(1);
	}
	return (fd);
}

int	handle_redirection_error(int *in_fd, int *out_fd)
{
	if (out_fd && *out_fd != STDOUT_FILENO)
		close(*out_fd);
	if (in_fd && *in_fd != STDIN_FILENO)
		close(*in_fd);
	return (-1);
}

int	get_output_flags(t_redirect *redir)
{
	if (redir->type == T_REDIR_OUT)
		return (O_WRONLY | O_CREAT | O_TRUNC);
	if (redir->type == T_REDIR_APPEND)
		return (O_WRONLY | O_CREAT | O_APPEND);
	ft_putstr_fd("borsh: unknown output redirection type\n", 2);
	set_last_exit_status(1);
	return (-1);
}

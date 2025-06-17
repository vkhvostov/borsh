#include "../../include/borsh.h"

static int	handle_input_redirection(t_redirect *redir, int *fd)
{
	int	temp_fd;

	if (redir->type == T_REDIR_IN)
	{
		temp_fd = open_file_with_flags(redir->file, O_RDONLY);
		if (temp_fd == -1)
			return (-1);
		*fd = temp_fd;
	}
	else if (redir->type == T_HEREDOC)
	{
		if (handle_heredoc(redir, &temp_fd) == -1)
			return (-1);
		*fd = temp_fd;
	}
	return (0);
}

static int	handle_output_redirection(t_redirect *redir, int *fd)
{
	int	temp_fd;
	int	flags;

	flags = get_output_flags(redir);
	if (flags == -1)
		return (-1);
	temp_fd = open_file_with_flags(redir->file, flags);
	if (temp_fd == -1)
		return (-1);
	*fd = temp_fd;
	return (0);
}

static int	process_input_redirection(t_redirect *r, int *in_fd, int *out_fd)
{
	close_fd_safe(in_fd);
	if (handle_input_redirection(r, in_fd) == -1)
		return (handle_redirection_error(in_fd, out_fd));
	return (0);
}

static int	process_output_redirection(t_redirect *r, int *in_fd, int *out_fd)
{
	close_fd_safe(out_fd);
	if (handle_output_redirection(r, out_fd) == -1)
		return (handle_redirection_error(in_fd, out_fd));
	return (0);
}

int	handle_redirections(t_command *command, int *in_fd, int *out_fd)
{
	t_redirect	*r;

	if (in_fd)
		*in_fd = STDIN_FILENO;
	if (out_fd)
		*out_fd = STDOUT_FILENO;
	r = command->redirs;
	while (r != NULL)
	{
		if (r->type == T_REDIR_IN || r->type == T_HEREDOC)
		{
			if (in_fd && process_input_redirection(r, in_fd, out_fd) == -1)
				return (-1);
		}
		else if (r->type == T_REDIR_OUT || r->type == T_REDIR_APPEND)
		{
			if (out_fd && process_output_redirection(r, in_fd, out_fd) == -1)
				return (-1);
		}
		r = r->next;
	}
	return (0);
}

#include "../../include/borsh.h"

static int	write_line_to_pipe(int pipe_fd, const char *line)
{
	if (write(pipe_fd, line, ft_strlen(line)) == -1)
	{
		ft_putstr_fd("borsh: write to heredoc pipe failed\n", 2);
		set_last_exit_status(1);
		return (-1);
	}
	if (write(pipe_fd, "\n", 1) == -1)
	{
		ft_putstr_fd("borsh: write newline to heredoc pipe failed\n", 2);
		set_last_exit_status(1);
		return (-1);
	}
	return (0);
}

static void	cleanup_heredoc(char *line, int *pipe_fds)
{
	if (line)
		free(line);
	if (pipe_fds)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
	}
}

static int	process_heredoc_line(char *line, char *delimiter, int write_fd)
{
	if (line == NULL)
	{
		ft_putstr_fd("borsh: here-document delimited by end-of-file", 2);
		ft_putstr_fd(" (wanted `", 2);
		ft_putstr_fd(delimiter, 2);
		ft_putstr_fd("')\n", 2);
		return (1);
	}
	if (ft_strcmp(line, delimiter) == 0)
	{
		free(line);
		return (1);
	}
	if (write_line_to_pipe(write_fd, line) == -1)
	{
		free(line);
		set_last_exit_status(1);
		return (-1);
	}
	free(line);
	return (0);
}

static int	read_heredoc_input(t_redirect *redir, int *pipefd)
{
	char	*line;
	int		result;

	while (1)
	{
		line = readline("> ");
		result = process_heredoc_line(line, redir->file, pipefd[1]);
		if (result != 0)
		{
			if (result == -1)
			{
				cleanup_heredoc(NULL, pipefd);
				set_last_exit_status(1);
				return (-1);
			}
			break ;
		}
	}
	return (0);
}

int	handle_heredoc(t_redirect *redir, int *heredoc_pipe_fd)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		ft_putstr_fd("borsh: pipe for heredoc failed\n", 2);
		set_last_exit_status(1);
		return (-1);
	}
	if (read_heredoc_input(redir, pipefd) == -1)
		return (-1);
	close(pipefd[1]);
	*heredoc_pipe_fd = pipefd[0];
	return (0);
}

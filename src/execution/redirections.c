#include "../../include/execution.h"

static int write_line_to_pipe(int pipe_fd, const char *line)
{
	if (write(pipe_fd, line, strlen(line)) == -1)
	{
		perror("write to heredoc pipe failed");
		return (-1);
	}
	if (write(pipe_fd, "\n", 1) == -1)
	{
		perror("write newline to heredoc pipe failed");
		return (-1);
	}
	return (0);
}

static void cleanup_heredoc(char *line, int *pipe_fds)
{
	if (line)
		free(line);
	if (pipe_fds)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
	}
}

static int process_heredoc_line(char *line, const char *delimiter, int write_fd)
{
	if (line == NULL)
	{
		fprintf(stderr, "borsh: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter);
		return (1);
	}

	if (strcmp(line, delimiter) == 0)
	{
		free(line);
		return (1);
	}

	if (write_line_to_pipe(write_fd, line) == -1)
	{
		free(line);
		return (-1);
	}

	free(line);
	return (0);
}

int handle_heredoc(t_redirect *redir, int *heredoc_pipe_fd)
{
	int pipefd[2];
	char *line;
	int result;

	if (pipe(pipefd) == -1)
	{
		perror("pipe for heredoc failed");
		return (-1);
	}

	// The prompt mentions readline "doesn't have to update history".
	// `rl_prep_terminal_for_heredoc ()` or similar might be needed if readline
	// is deeply integrated. For now, standard readline usage.
	// To avoid history, one might use `add_history(NULL)` before and restore,
	// or more simply, just not call `add_history` if `readline` doesn't do it by default
	// when not explicitly called. `readline` itself does add to history.
	// A simple solution is to clear history for this specific use if that's acceptable.
	// Or, use `rl_clear_signals()` if signals interfere.

	// For now, let's assume standard readline behavior is acceptable,
	// and we can refine history management later if needed.

	while (1)
	{
		line = readline("> ");
		result = process_heredoc_line(line, redir->file, pipefd[1]);
		if (result != 0)
		{
			if (result == -1)
			{
				cleanup_heredoc(NULL, pipefd);
				return (-1);
			}
			break;
		}
	}

	close(pipefd[1]);
	*heredoc_pipe_fd = pipefd[0];
	return (0);
}

static int handle_input_redirection(t_redirect *redir, int *fd)
{
	int temp_fd;

	if (redir->type == T_REDIR_IN)
	{
		temp_fd = open(redir->file, O_RDONLY);
		if (temp_fd == -1)
		{
			fprintf(stderr, "borsh: %s: %s\n", redir->file, strerror(errno));
			return (-1);
		}
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

static int handle_output_redirection(t_redirect *redir, int *fd)
{
	int temp_fd;

	if (redir->type == T_REDIR_OUT)
		temp_fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (redir->type == T_REDIR_APPEND)
		temp_fd = open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
	{
		fprintf(stderr, "borsh: unknown output redirection type\n");
		return (-1);
	}
	
	if (temp_fd == -1)
	{
		fprintf(stderr, "borsh: %s: %s\n", redir->file, strerror(errno));
		return (-1);
	}
	*fd = temp_fd;
	return (0);
}

static void close_previous_fd(int *fd)
{
	if (*fd != STDIN_FILENO && *fd != STDOUT_FILENO)
		close(*fd);
}

int handle_redirections(t_command *command, int *in_fd, int *out_fd)
{
	t_redirect *current_redir;

	if (in_fd)
	{
		*in_fd = STDIN_FILENO;
		current_redir = command->in_redir;
		while (current_redir != NULL)
		{
			close_previous_fd(in_fd);
			if (handle_input_redirection(current_redir, in_fd) == -1)
			{
				if (out_fd && *out_fd != STDOUT_FILENO)
					close(*out_fd);
				return (-1);
			}
			current_redir = current_redir->next;
		}
	}

	if (out_fd)
	{
		*out_fd = STDOUT_FILENO;
		current_redir = command->out_redir;
		while (current_redir != NULL)
		{
			close_previous_fd(out_fd);
			if (handle_output_redirection(current_redir, out_fd) == -1)
			{
				if (in_fd && *in_fd != STDIN_FILENO)
					close(*in_fd);
				return (-1);
			}
			current_redir = current_redir->next;
		}
	}
	return (0);
}

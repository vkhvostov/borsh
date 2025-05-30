#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h> // For readline
// borsh.h should ideally provide libft functions and g_exit_status
#include "borsh.h"

// Global variable for exit status (expected to be in borsh.h)
extern int	g_exit_status;

// Forward declaration for variable expansion function (assumed from lexer)
// If this function needs envp, its signature and definition elsewhere must match.
char	*expand_variables(const char *input);

/**
 * @brief Handles here-document redirection (<< delimiter).
 *
 * Reads input lines until the delimiter is encountered, performs variable
 * expansion on each line, and writes the result to a pipe. The read end
 * of this pipe is then duplicated to STDIN_FILENO.
 *
 * @param delimiter The delimiter string for the here-document.
 * @param envp Environment variables, potentially for variable expansion.
 * @return 0 on success, -1 on failure. Sets g_exit_status on failure.
 */
int	handle_heredoc(const char *delimiter, char **envp)
{
	int		temp_pipe[2];
	char	*line;
	char	*expanded_line;

	(void)envp; // Currently unused if expand_variables doesn't take envp.

	if (!delimiter)
	{
		ft_putstr_fd("minishell: syntax error: missing here-document delimiter\n",
			STDERR_FILENO);
		g_exit_status = 2; // Bash uses 2 for syntax errors
		return (-1);
	}
	if (pipe(temp_pipe) == -1)
	{
		perror("minishell: pipe");
		g_exit_status = 1;
		return (-1);
	}
	while (1)
	{
		line = readline("> ");
		if (!line) // Ctrl-D or EOF
		{
			ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted `", STDERR_FILENO);
			ft_putstr_fd((char *)delimiter, STDERR_FILENO); // Cast if ft_putstr_fd needs char*
			ft_putstr_fd("')\n", STDERR_FILENO);
			break ;
		}
		// Delimiter check: must be exact match on its own line
		if (ft_strlen(line) == ft_strlen(delimiter)
			&& ft_strncmp(line, delimiter, ft_strlen(delimiter)) == 0)
		{
			free(line);
			break ;
		}
		// Variable expansion (assuming expand_variables is available)
		// If expand_variables needs envp, its signature must be char *expand_variables(const char *input, char **envp)
		// For now, using the signature from the forward declaration.
		expanded_line = expand_variables(line);
		free(line); // Original line from readline is no longer needed
		if (!expanded_line)
		{
			perror("minishell: expand_variables"); // Malloc error or other
			// Decide if we should continue or error out the heredoc.
			// For now, let's assume it might return NULL on error and we write nothing.
			// Or, if it returns input on error, that's fine too.
			// If it's critical, close pipe ends and return -1.
			// For simplicity, let's assume expand_variables handles its errors or returns non-NULL.
		}

		if (expanded_line)
		{
			if (write(temp_pipe[1], expanded_line, ft_strlen(expanded_line)) == -1)
			{
				perror("minishell: write to heredoc pipe");
				free(expanded_line);
				close(temp_pipe[0]); // Close read end as well
				close(temp_pipe[1]);
				g_exit_status = 1;
				return (-1);
			}
			if (write(temp_pipe[1], "\n", 1) == -1)
			{
				perror("minishell: write newline to heredoc pipe");
				free(expanded_line);
				close(temp_pipe[0]);
				close(temp_pipe[1]);
				g_exit_status = 1;
				return (-1);
			}
			free(expanded_line);
		}
	}
	close(temp_pipe[1]); // Close write end, EOF for reader on the other side
	if (dup2(temp_pipe[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 for heredoc STDIN");
		close(temp_pipe[0]);
		g_exit_status = 1;
		return (-1);
	}
	close(temp_pipe[0]); // Close original read end
	return (0);
}

#include "borsh.h" // For t_token, T_PIPE, g_exit_status, common headers (unistd, stdio, stdlib, sys/wait, string), and libft functions.

// Forward declarations (eventually to be in borsh.h or a specific exec header)
int		execute_command(t_token *command_token, char **envp);
// launch_process is not directly called by handle_pipe.

// Global variable for exit status (defined elsewhere, e.g., main.c or global.c)
extern int	g_exit_status;

// Helper to find the first pipe token and the token before it.
// *left_cmd_end will point to the token whose 'next' should be NULL for left cmd.
// Returns the pipe token itself, or NULL if no pipe.
static t_token	*find_pipe_token_and_prev(t_token *tokens, t_token **left_cmd_end)
{
	t_token	*current;
	t_token	*prev;

	current = tokens;
	prev = NULL;
	*left_cmd_end = NULL; 

	while (current != NULL)
	{
		if (current->type == T_PIPE)
		{
			*left_cmd_end = prev; // This is the token before the pipe.
			return (current);     // This is the pipe token.
		}
		prev = current;
		current = current->next;
	}
	return (NULL); // No pipe found.
}

/**
 * @brief Handles command execution involving pipes.
 *
 * Recursively sets up pipes and forks processes for commands separated by T_PIPE.
 * If no pipe is found, it executes the command directly.
 * The exit status of the pipeline is the exit status of the rightmost command.
 *
 * @param tokens The linked list of tokens representing the command(s).
 * @param envp The environment variables.
 * @return The exit status of the last command in the pipeline, or an error code
 *         if a pipe or fork fails. This return value is also stored in g_exit_status.
 */
int	handle_pipe(t_token *tokens, char **envp)
{
	t_token	*pipe_token;
	t_token	*left_command_last_token; 
	t_token	*right_command_tokens;
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;
	int		status1; // Status of the left child
	int		status2; // Status of the right child (or recursive pipeline)

	if (tokens == NULL) // Base case: empty command segment
	{
		g_exit_status = 0; // No command given, typically results in success.
		return (0);
	}

	pipe_token = find_pipe_token_and_prev(tokens, &left_command_last_token);

	// Base Case: No pipe found, execute as a single command.
	if (pipe_token == NULL)
	{
		execute_command(tokens, envp); // This function should set g_exit_status.
		return (g_exit_status);
	}

	// Pipe found. Validate syntax first.
	// Case: `| cmd` (pipe is the first effective token for this segment)
	if (left_command_last_token == NULL) 
	{
		// Assuming 'tokens' points directly to T_PIPE or 'tokens' is the command
		// and find_pipe_token_and_prev handles it. If tokens == pipe_token:
		ft_putstr_fd("borsh: syntax error near unexpected token `|'\n", STDERR_FILENO);
		g_exit_status = 2; // Standard syntax error code
		return (2);
	}
	// Case: `cmd |` (pipe is the last token)
	if (pipe_token->next == NULL)
	{
		ft_putstr_fd("borsh: syntax error near unexpected token `|'\n", STDERR_FILENO);
		g_exit_status = 2;
		return (2);
	}
	right_command_tokens = pipe_token->next;

	if (pipe(pipefd) == -1)
	{
		perror("borsh: pipe");
		g_exit_status = 1; // System call error
		return (1);
	}

	pid1 = fork(); // Fork for the left side command
	if (pid1 == -1)
	{
		perror("borsh: fork");
		close(pipefd[0]);
		close(pipefd[1]);
		g_exit_status = 1;
		return (1);
	}

	if (pid1 == 0) // Child 1 (executes left side of the pipe)
	{
		close(pipefd[0]); // Child 1 does not read from the pipe
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			perror("borsh: dup2 for stdout");
			close(pipefd[1]); // Close the write end before exiting
			exit(EXIT_FAILURE); // Critical error
		}
		close(pipefd[1]); // STDOUT_FILENO is now a copy of pipefd[1]

		// Isolate the left command's tokens
		left_command_last_token->next = NULL;
		
		execute_command(tokens, envp); // tokens now represents only the left command
		exit(g_exit_status); // Child exits with the status of the executed command
	}

	pid2 = fork(); // Fork for the right side command (or next segment)
	if (pid2 == -1)
	{
		perror("borsh: fork");
		close(pipefd[0]); // Must close these in parent on error
		close(pipefd[1]);
		waitpid(pid1, NULL, 0); // Reap the first child if second fork fails
		g_exit_status = 1;
		return (1);
	}

	if (pid2 == 0) // Child 2 (executes right side of the pipe)
	{
		close(pipefd[1]); // Child 2 does not write to this pipe
		if (dup2(pipefd[0], STDIN_FILENO) == -1)
		{
			perror("borsh: dup2 for stdin");
			close(pipefd[0]); // Close the read end before exiting
			exit(EXIT_FAILURE); // Critical error
		}
		close(pipefd[0]); // STDIN_FILENO is now a copy of pipefd[0]
		
		handle_pipe(right_command_tokens, envp); // Recursively handle the rest
		exit(g_exit_status); // Child exits with status from its command execution
	}

	// Parent process continues here
	// Crucially, parent must close *both* ends of the pipe after children are forked
	close(pipefd[0]);
	close(pipefd[1]);

	// Wait for both children to complete. Order can matter for $? in some shells.
	// Bash uses the exit status of the rightmost command in a pipeline.
	waitpid(pid1, &status1, 0); // Wait for left child
	waitpid(pid2, &status2, 0); // Wait for right child

	// Set global exit status based on the rightmost command's termination
	if (WIFEXITED(status2))
		g_exit_status = WEXITSTATUS(status2);
	else if (WIFSIGNALED(status2))
		g_exit_status = 128 + WTERMSIG(status2);
	// Else: Could be stopped, continued, or some other state.
	// For a simple shell, exited/signaled are primary.
	// If pid2 failed to exec, g_exit_status might reflect that from its own exit.

	// The token list modification (left_command_last_token->next = NULL)
	// was in child1's address space, so parent's list is intact.
	return (g_exit_status);
}

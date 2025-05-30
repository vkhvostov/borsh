#include "borsh.h" // Should provide most system headers and t_token, g_exit_status

// For system calls like dup, dup2, close, fork, pipe, access, execve if not fully
// transparently included via borsh.h, though previous refactoring aimed for this.
// #include <unistd.h>
// #include <fcntl.h> // For open and flags if used directly here
// #include <sys/wait.h> // For waitpid if used directly here

// Global variable for exit status
extern int	g_exit_status;

// Forward declarations from other execution files
int		handle_pipe(t_token *tokens, char **envp);
int		handle_heredoc(const char *delimiter, char **envp);
int		handle_redir_in(const char *filename);
int		handle_redir_out(const char *filename);
int		handle_redir_append(const char *filename);
char	**build_argv_from_tokens(t_token *start_token, t_token **end_token);
int		execute_command(t_token *command_tokens, char **envp);
// Note: execute_command internally calls launch_process.

// ---- Data Structures for Command Execution ----
// These structures might be moved to borsh.h if they are needed by other
// modules like piping.c extensively. For now, defined locally.

typedef struct s_redirection {
	t_token_type		type;         // T_REDIR_IN, T_REDIR_OUT, T_REDIR_APPEND
	char				*filename;    // Filename for the redirection
	struct s_redirection *next;
}	t_redirection;

typedef struct s_command_data {
	t_token			*arg_tokens;      // Linked list of T_WORD tokens (command and arguments)
	t_redirection	*redirections;    // Linked list of redirections
	char			*heredoc_delimiter; // Delimiter if T_HEREDOC is present
	// int             heredoc_fd;       // Pipe read end from handle_heredoc
	                                  // This will be managed locally by execute_simple_command
}	t_command_data;


// ---- Helper Functions for Memory Management ----

void	free_redirection_list(t_redirection *list)
{
	t_redirection	*current;
	t_redirection	*next;

	current = list;
	while (current != NULL)
	{
		next = current->next;
		free(current->filename); // Filename was strdup'd
		free(current);
		current = next;
	}
}

void	free_command_data(t_command_data *cmd_data)
{
	if (!cmd_data)
		return ;
	if (cmd_data->arg_tokens)
	{
		// Assuming arg_tokens is a new list that needs freeing.
		// free_tokens is from lexer part, make sure it's declared/available if used.
		// For now, let's assume it's a flat list of T_WORDs and we only need to free the list structure,
		// but not individual token values if they point to original parser output (which is unlikely for this design)
		// If extract_command_tokens strdup's token values for arg_tokens, then free_tokens should handle that.
		// Let's assume free_tokens is the correct way to free a token list.
		free_tokens(cmd_data->arg_tokens); 
		cmd_data->arg_tokens = NULL;
	}
	if (cmd_data->redirections)
	{
		free_redirection_list(cmd_data->redirections);
		cmd_data->redirections = NULL;
	}
	if (cmd_data->heredoc_delimiter)
	{
		free(cmd_data->heredoc_delimiter); // Delimiter was strdup'd
		cmd_data->heredoc_delimiter = NULL;
	}
	// heredoc_fd is just an int, no freeing needed.
}

// ---- Core Logic Functions ----

/**
 * @brief Extracts command arguments, redirections, and heredoc delimiter
 * from a token list segment.
 *
 * Iterates through tokens from current_token until a T_PIPE or the end of list.
 * - T_WORD tokens are duplicated and added to cmd_data->arg_tokens.
 * - Redirection tokens (T_REDIR_*, T_HEREDOC) and their associated filenames/
 *   delimiters are duplicated and stored in cmd_data->redirections or
 *   cmd_data->heredoc_delimiter.
 *
 * @param current_token The start of the token segment to process.
 * @param cmd_data      Pointer to a t_command_data struct to populate.
 * @return t_token*     Pointer to the token that stopped processing (T_PIPE or NULL).
 */
t_token	*extract_command_tokens(t_token *current_token, t_command_data *cmd_data)
{
	t_token			*token_iter;
	t_token			*new_arg_token;
	t_token			*last_arg_token;
	t_redirection	*new_redir;
	t_redirection	*last_redir;

	if (!cmd_data)
		return (current_token); // Should not happen
	
	// Initialize cmd_data fields to NULL
	cmd_data->arg_tokens = NULL;
	cmd_data->redirections = NULL;
	cmd_data->heredoc_delimiter = NULL;
	last_arg_token = NULL;
	last_redir = NULL;

	token_iter = current_token;
	while (token_iter != NULL && token_iter->type != T_PIPE)
	{
		if (token_iter->type == T_WORD)
		{
			new_arg_token = (t_token *)malloc(sizeof(t_token));
			if (!new_arg_token) { /* TODO: Malloc error handling */ return NULL; }
			new_arg_token->value = ft_strdup(token_iter->value);
			if (!new_arg_token->value && token_iter->value) { /* TODO: Malloc error */ free(new_arg_token); return NULL; }
			new_arg_token->type = T_WORD;
			new_arg_token->next = NULL;
			if (last_arg_token)
				last_arg_token->next = new_arg_token;
			else
				cmd_data->arg_tokens = new_arg_token;
			last_arg_token = new_arg_token;
		}
		else if (token_iter->type == T_REDIR_IN || token_iter->type == T_REDIR_OUT || \
		         token_iter->type == T_REDIR_APPEND || token_iter->type == T_HEREDOC)
		{
			t_token_type redir_type = token_iter->type;
			token_iter = token_iter->next; // Move to filename/delimiter
			if (!token_iter || token_iter->type != T_WORD) // Expecting a filename/delimiter
			{
				ft_putstr_fd("minishell: syntax error near unexpected token\n", STDERR_FILENO);
				g_exit_status = 2;
				// Cleanup allocated data in cmd_data before returning error.
				free_command_data(cmd_data); 
				// To signal error properly, this function should return NULL and set status,
				// or the caller checks g_exit_status. For now, returning NULL for simplicity.
				return (NULL); 
			}
			
			if (redir_type == T_HEREDOC)
			{
				if (cmd_data->heredoc_delimiter) // Overwrite previous heredoc for this command
					free(cmd_data->heredoc_delimiter);
				cmd_data->heredoc_delimiter = ft_strdup(token_iter->value);
				if (!cmd_data->heredoc_delimiter) { /* TODO: Malloc error */ return NULL; }
			}
			else
			{
				new_redir = (t_redirection *)malloc(sizeof(t_redirection));
				if (!new_redir) { /* TODO: Malloc error handling */ return NULL; }
				new_redir->type = redir_type;
				new_redir->filename = ft_strdup(token_iter->value);
				if (!new_redir->filename && token_iter->value) { /* TODO: Malloc error */ free(new_redir); return NULL; }
				new_redir->next = NULL;
				if (last_redir)
					last_redir->next = new_redir;
				else
					cmd_data->redirections = new_redir;
				last_redir = new_redir;
			}
		}
		else // Should not happen if parser is correct (e.g. multiple pipes `||`)
		{
			ft_putstr_fd("minishell: internal parser error or unhandled token type in execute\n", STDERR_FILENO);
			g_exit_status = 255; // Internal error
			free_command_data(cmd_data);
			return (NULL);
		}
		if (token_iter) // token_iter could be NULL if redir was last token
			token_iter = token_iter->next;
	}
	return (token_iter); // This is either T_PIPE or NULL
}

/**
 * @brief Applies file redirections (input, output, append) and heredoc input.
 *
 * @param cmd_data  Command data containing redirection information.
 * @param heredoc_fd Read end of the pipe from handle_heredoc, or -1 if no heredoc.
 * @return 0 on success, -1 on failure.
 */
int	apply_redirections(t_command_data *cmd_data, int heredoc_fd)
{
	t_redirection	*redir;
	int				heredoc_stdin_applied;

	heredoc_stdin_applied = 0;
	if (heredoc_fd != -1)
	{
		if (dup2(heredoc_fd, STDIN_FILENO) == -1)
		{
			perror("minishell: dup2 for heredoc_fd");
			close(heredoc_fd); // Close it even on error
			g_exit_status = 1;
			return (-1);
		}
		close(heredoc_fd); // Original heredoc pipe fd is now closed
		heredoc_stdin_applied = 1;
	}

	redir = cmd_data->redirections;
	while (redir != NULL)
	{
		if (redir->type == T_REDIR_IN)
		{
			if (heredoc_stdin_applied) { /* Heredoc takes precedence for STDIN */ }
			else if (handle_redir_in(redir->filename) == -1)
				return (-1);
		}
		else if (redir->type == T_REDIR_OUT)
		{
			if (handle_redir_out(redir->filename) == -1)
				return (-1);
		}
		else if (redir->type == T_REDIR_APPEND)
		{
			if (handle_redir_append(redir->filename) == -1)
				return (-1);
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * @brief Executes a single command segment (no pipes within this function).
 * Handles heredocs, applies redirections, builds argv, and executes.
 *
 * @param tokens The token list for this simple command.
 * @param envp Environment variables.
 * @return Exit status of the command.
 */
int	execute_simple_command(t_token *tokens, char **envp)
{
	t_command_data	cmd_data;
	int				heredoc_fd;
	// t_token        *next_segment_start; // Not used here, but extract_command_tokens returns it

	ft_memset(&cmd_data, 0, sizeof(t_command_data)); // Initialize to zero/NULL
	heredoc_fd = -1;

	// next_segment_start = extract_command_tokens(tokens, &cmd_data);
    // If extract_command_tokens returns NULL on error:
    if (extract_command_tokens(tokens, &cmd_data) == NULL && g_exit_status != 0) {
        // Error occurred in extract_command_tokens (e.g. syntax, malloc)
        // cmd_data should have been freed by extract_command_tokens on its error path.
        return (g_exit_status);
    }


	if (cmd_data.heredoc_delimiter)
	{
		heredoc_fd = handle_heredoc(cmd_data.heredoc_delimiter, envp);
		if (heredoc_fd == -1)
		{
			free_command_data(&cmd_data);
			return (g_exit_status); // handle_heredoc sets g_exit_status
		}
	}

	if (apply_redirections(&cmd_data, heredoc_fd) == -1)
	{
		// If heredoc_fd was used, apply_redirections closes it.
		// If it wasn't used (e.g. apply_redirections failed before using it),
		// and it's still open, it should be closed. But apply_redirections
		// should handle closing it if it took ownership via dup2.
		// The current apply_redirections closes heredoc_fd after successful dup2.
		// If dup2 fails, it also closes it. So heredoc_fd should be managed there.
		free_command_data(&cmd_data);
		return (g_exit_status); // apply_redirections sets g_exit_status
	}
    
    // If there are no command arguments (e.g. " > out" or just redirections)
    // but redirections were successful, g_exit_status should be 0.
    if (cmd_data.arg_tokens == NULL) {
        free_command_data(&cmd_data);
        // g_exit_status might be 1 if a redirection failed and was handled above.
        // If all redirections succeeded, g_exit_status should be 0.
        // The subshell for redirections like `> outfile` in bash returns 0.
        // Assuming successful redirection means g_exit_status is not 1 at this point.
        return (0); 
    }

	execute_command(cmd_data.arg_tokens, envp); // This sets g_exit_status
	
	free_command_data(&cmd_data);
	return (g_exit_status);
}


/**
 * @brief Main entry point for command execution.
 * Manages overall execution flow, including pipes and final status.
 *
 * @param tokens The full list of tokens from the parser.
 * @param envp Environment variables.
 * @return The final exit status of the command or pipeline.
 */
int	execute(t_token *tokens, char **envp)
{
	int	saved_stdin;
	int	saved_stdout;

	g_exit_status = 0; // Initialize for the new command line

	// Save original STDIN and STDOUT
	saved_stdin = dup(STDIN_FILENO);
	if (saved_stdin == -1) {
		perror("minishell: dup saved_stdin");
		return (g_exit_status = 1, 1);
	}
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdout == -1) {
		perror("minishell: dup saved_stdout");
		close(saved_stdin);
		return (g_exit_status = 1, 1);
	}

	// For now, handle_pipe is the main driver.
	// It will need to be adapted to use execute_simple_command for its children.
	// Or, the logic of handle_pipe will be merged/re-written here.
	// The current handle_pipe calls execute_command, which is not ideal with the new structure.
	// This is a placeholder for the full pipeline execution logic.
	// A refactored handle_pipe would fork and then call a function like
	// execute_simple_command_in_child_context(cmd_data, pipe_in_fd, pipe_out_fd, envp)
	handle_pipe(tokens, envp);


	// Restore original STDIN and STDOUT
	if (dup2(saved_stdin, STDIN_FILENO) == -1) {
		perror("minishell: dup2 restore STDIN_FILENO");
		// Continue to close, but major issue
	}
	close(saved_stdin);

	if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
		perror("minishell: dup2 restore STDOUT_FILENO");
		// Continue to close, but major issue
	}
	close(saved_stdout);

	return (g_exit_status);
}

#include "../include/borsh.h"

// Ensure these are declared, typically in borsh.h or a specific parser header
// For this task, we've added them to borsh.h:
// void reset_parser_error_flag(void);
// int has_parser_error_occurred(void);
// void handle_parser_error(char *message); // Already there, behavior changed
// void free_commands(t_command *cmd); // Assumed to be in borsh.h

t_command	*init_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->cmd_name = NULL;
	cmd->argv = NULL;
	cmd->in_redir = NULL;
	cmd->out_redir = NULL;
	cmd->next = NULL;
	return (cmd);
}

int	add_arg(char ***argv, char *value)
{
	int		len;
	char	**new_argv;
	char	*new_arg_val;
	int		i;

	new_arg_val = ft_strdup(value);
	if (!new_arg_val)
		return (0);

	len = 0;
	if (*argv)
		while ((*argv)[len])
			len++;

	new_argv = malloc(sizeof(char *) * (len + 2));
	if (!new_argv)
	{
		free(new_arg_val);
		return (0);
	}

	i = 0;
	while (i < len)
	{
		new_argv[i] = (*argv)[i];
		i++;
	}

	new_argv[len] = new_arg_val;
	new_argv[len + 1] = NULL;

	free(*argv);
	*argv = new_argv;
	return (1);
}

int	is_redirect(t_token *tokens)
{
	return (tokens->type == T_REDIR_IN
				|| tokens->type == T_REDIR_OUT
				|| tokens->type == T_REDIR_APPEND
				|| tokens->type == T_HEREDOC);
}

void	handle_redirs(t_token **tokens, t_command	*current)
{
	if ((*tokens)->type == T_REDIR_IN)
		handle_redir_tokens(&current->in_redir, tokens, T_REDIR_IN);
	else if ((*tokens)->type == T_REDIR_OUT)
		handle_redir_tokens(&current->out_redir, tokens, T_REDIR_OUT);
	else if ((*tokens)->type == T_REDIR_APPEND)
		handle_redir_tokens(&current->out_redir, tokens, T_REDIR_APPEND);
	else if ((*tokens)->type == T_HEREDOC)
		handle_redir_tokens(&current->in_redir, tokens, T_HEREDOC);
}

t_command	*parse_tokens(t_token *tokens)
{
	t_command	*head = NULL;
	t_command	*current = NULL;

	reset_parser_error_flag();

	if (!tokens) // Handle empty token list (e.g. empty line or lexer error)
	{
		if (has_parser_error_occurred()) // If lexer itself set an error
		{
			// No commands to free yet.
			return (NULL);
		}
		return (NULL); // Normal empty line
	}

	while (tokens)
	{
		if (!current)
		{
			current = init_command();
			if (!current) // init_command failure
			{
				handle_parser_error("memory allocation failed for command structure");
				// head is NULL or points to an already freed/problematic area if error occurred before
				// If current is NULL, head is also NULL if this is the first command.
				// If not the first command, previous commands are in head.
				free_commands(head); 
				return (NULL);
			}
			if (!head)
				head = current;
		}

		// General check at the start of each token processing iteration
		if (has_parser_error_occurred())
		{
			free_commands(head);
			return (NULL);
		}

		if (tokens->type == T_PIPE)
		{
			// Pipe specific syntax checks
			if ((current == head && !current->cmd_name && !current->argv) || // Pipe at start or after empty command
			    tokens->next == NULL || tokens->next->type == T_PIPE) // Pipe at end or before another pipe
			{
				if (current == head && !current->cmd_name && !current->argv)
					handle_parser_error("Syntax error: pipe at beginning of command or after empty command segment");
				else
					handle_parser_error("Syntax error: invalid null command near pipe");
				
				free_commands(head);
				return (NULL);
			}
			handle_pipe_tokens(&tokens, &current); // Advances tokens, current becomes new command
			if (has_parser_error_occurred()) // Check if init_command inside handle_pipe_tokens failed
			{
				free_commands(head);
				return (NULL);
			}
			// tokens is advanced by handle_pipe_tokens. Loop continues.
			continue; 
		}
		else if (tokens->type == T_WORD)
		{
			if (!handle_word_tokens(current, tokens)) // This function calls handle_parser_error on failure
			{
				// Error flag is set by handle_word_tokens
				free_commands(head);
				return (NULL);
			}
			tokens = tokens->next; // Manually advance token for T_WORD
		}
		else if (is_redirect(tokens))
		{
			handle_redirs(&tokens, current); // Advances tokens (or should)
			if (has_parser_error_occurred()) // Check if add_redirect or filename checks failed
			{
				free_commands(head);
				return (NULL);
			}
			// tokens is advanced by handle_redirs on success.
			// If error in handle_redirs (e.g. missing filename), token might not advance to prevent infinite loop.
			// The has_parser_error_occurred() check above should catch this.
		}
		else // Should not be reached if lexer is correct and all token types are handled.
		{
			handle_parser_error("Unknown or unexpected token type encountered in parser");
			free_commands(head);
			return (NULL);
		}
	}

	// After loop, check if the last command initiated by a pipe is empty.
	// (e.g. "cmd |" and then EOF)
	// This case should be caught by "tokens->next == NULL" in T_PIPE block if pipe is last token.
	// If current command is not head, and it's empty, it implies a trailing pipe.
	if (current && current != head && !current->cmd_name && !current->argv && !current->in_redir && !current->out_redir)
	{
		handle_parser_error("Syntax error: command expected after pipe");
		free_commands(head);
		return (NULL);
	}
	
	// Final overall error check before returning successfully parsed commands
	if (has_parser_error_occurred())
	{
		free_commands(head);
		return (NULL);
	}

	return (head);
}

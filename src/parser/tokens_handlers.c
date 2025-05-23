#include "../../include/borsh.h"
#include <stdio.h> // For fprintf

static int	g_parser_error_occurred = 0;

void	reset_parser_error_flag(void)
{
	g_parser_error_occurred = 0;
}

int	has_parser_error_occurred(void)
{
	return (g_parser_error_occurred);
}

void	add_redirect(t_redirect **list, t_token_type type, char *file)
{
	t_redirect	*new;
	t_redirect	*last;

	new = malloc(sizeof(t_redirect));
	if (!new)
	{
		handle_parser_error("memory allocation failed for redirect structure");
		return ;
	}
	new->type = type;
	new->file = ft_strdup(file);
	if (!new->file)
	{
		handle_parser_error("memory allocation failed for redirect file name");
		free(new);
		return ;
	}
	new->next = NULL;
	if (!*list)
		*list = new;
	else
	{
		last = *list;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

void	handle_parser_error(char *message)
{
	fprintf(stderr, "Syntax error: %s\n", message);
	g_parser_error_occurred = 1;
}

void	handle_redir_tokens(t_redirect **redir_list, t_token **tokens,
							t_token_type type)
{
	if ((*tokens)->next == NULL || (*tokens)->next->type != T_WORD)
	{
		if ((*tokens)->type == T_REDIR_IN)
			handle_parser_error("missing file for input redirection");
		else if ((*tokens)->type == T_REDIR_OUT)
			handle_parser_error("missing file for output redirection");
		else if ((*tokens)->type == T_REDIR_APPEND)
			handle_parser_error("missing file for append redirection");
		else if ((*tokens)->type == T_HEREDOC)
			handle_parser_error("missing delimiter for heredoc");
		// The error flag is set by handle_parser_error.
		// parse_tokens will check this flag.
		// We need to advance the token to avoid an infinite loop if the caller doesn't.
		// However, the problem description implies parse_tokens will handle termination.
		// Let's ensure we advance *tokens if its next is the problematic one.
		if ((*tokens)->next && (*tokens)->next->type != T_WORD) {
			// This case is tricky; the error is about (*tokens)->next not being a WORD.
			// If we advance tokens, we might skip over other processing.
			// For now, let's rely on parse_tokens to see the flag and stop.
		}
	}
	else if ((*tokens)->next) // next token exists and is implicitly a T_WORD
	{
		add_redirect(redir_list, type, (*tokens)->next->value);
		// if add_redirect fails, it calls handle_parser_error and sets the flag.
	}
	// Advance the token pointer past the redirection keyword and its argument (filename)
	// if there was no error related to missing filename.
	if (!has_parser_error_occurred() && (*tokens)->next)
		*tokens = (*tokens)->next;
	// If error occurred (e.g. missing filename), do not advance *tokens past the filename,
	// as (*tokens)->next might be NULL or an unexpected token.
	// parse_tokens will stop due to the flag.
}


void handle_pipe_tokens(t_token **tokens, t_command **current)
{
	(*current)->next = init_command();
	if (!(*current)->next)
	{
		handle_parser_error("memory allocation failed for new command after pipe");
		return; // parse_tokens will check the flag and cleanup
	}
	*current = (*current)->next;
	
	// Advance token past T_PIPE. parse_tokens checks for invalid pipes (pipe at end, pipe -> pipe)
	// *before* calling this, so tokens->next should be valid here.
	// If tokens->next was NULL or another PIPE, parse_tokens would have already errored out.
	if ((*tokens)->next) // Should always be true due to prior checks in parse_tokens
		*tokens = (*tokens)->next;
	// If somehow tokens->next is NULL here, it's an unexpected state,
	// but parse_tokens' main loop condition (while (tokens)) will handle it.
}

int	handle_word_tokens(t_command	*current, t_token *tokens)
{
	int cmd_name_allocated_in_this_call = 0;

	if (!current->cmd_name)
	{
		current->cmd_name = ft_strdup(tokens->value);
		if (!current->cmd_name)
		{
			handle_parser_error("memory allocation failed for command name");
			return (0); 
		}
		cmd_name_allocated_in_this_call = 1;
	}

	if (!add_arg(&current->argv, tokens->value))
	{
		// add_arg itself does not call handle_parser_error on ft_strdup failure
		// so we must do it here.
		handle_parser_error("memory allocation failed for command argument");
		if (cmd_name_allocated_in_this_call)
		{
			free(current->cmd_name);
			current->cmd_name = NULL;
		}
		return (0);
	}
	return (1); // Success
}

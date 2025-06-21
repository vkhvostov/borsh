#include "../../include/borsh.h"

static void	add_redirect(t_redirect **list, t_token_type type, char *file,
	int *exit_status)
{
	t_redirect	*new;
	t_redirect	*last;

	new = malloc(sizeof(t_redirect));
	if (!new)
		memory_error_handler(exit_status);
	new->type = type;
	new->file = ft_strdup(file);
	if (!new->file)
	{
		free(new);
		memory_error_handler(exit_status);
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

static void	handle_parser_error(char *message, int *exit_status)
{
	printf("borsh: syntax error: %s\n", message);
	*exit_status = 2;
}

void	handle_redir_tokens(t_redirect **redir_list, t_token **tokens,
		t_token_type type, int *exit_status)
{
	if ((*tokens)->next == NULL || (*tokens)->next->type != T_WORD)
	{
		if ((*tokens)->type == T_REDIR_IN)
			handle_parser_error("missing file for input redirection",
				exit_status);
		else if ((*tokens)->type == T_REDIR_OUT)
			handle_parser_error("missing file for output redirection",
				exit_status);
		else if ((*tokens)->type == T_REDIR_APPEND)
			handle_parser_error("missing file for append redirection",
				exit_status);
		else if ((*tokens)->type == T_HEREDOC)
			handle_parser_error("missing delimiter for heredoc",
				exit_status);
		return ;
	}
	else if ((*tokens)->next)
		add_redirect(redir_list, type, (*tokens)->next->value, exit_status);
	*tokens = (*tokens)->next;
}

void	handle_pipe_tokens(t_token **tokens, t_command **current, char **env,
		int *exit_status)
{
	t_command	*new_cmd;

	if ((*tokens)->next == NULL)
	{
		handle_parser_error("unexpected token '|'", exit_status);
		return ;
	}
	new_cmd = init_command(env, exit_status);
	if (!new_cmd)
	{
		memory_error_handler(exit_status);
	}
	(*current)->next = new_cmd;
	*current = new_cmd;
	*tokens = (*tokens)->next;
}

int	handle_word_tokens(t_command *current, t_token *tokens, int *exit_status)
{
	if (!current->cmd_name)
		current->cmd_name = ft_strdup(tokens->value);
	if (!current->cmd_name)
	{
		free_argv(current->argv);
		current->argv = NULL;
		*exit_status = 1;
		return (0);
	}
	if (!add_arg(&current->argv, tokens->value, exit_status))
	{
		free(current->cmd_name);
		current->cmd_name = NULL;
		free_argv(current->argv);
		current->argv = NULL;
		*exit_status = 1;
		return (0);
	}
	return (1);
}

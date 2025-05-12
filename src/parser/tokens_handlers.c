#include "../../include/borsh.h"

void	add_redirect(t_redirect **list, t_redirect_type type, char *file)
{
	t_redirect	*new;
	t_redirect	*last;

	new = malloc(sizeof(t_redirect));
	if (!new)
		return ;
	new->type = type;
	new->file = ft_strdup(file);
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
	printf("Syntax error: %s\n", message);
	exit(1);
}

void	handle_redir_tokens(t_redirect **redir_list, t_token **tokens, 
							t_redirect_type type)
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
	}
	else if ((*tokens)->next)
		add_redirect(redir_list, type, (*tokens)->next->value);
	*tokens = (*tokens)->next;
}


void handle_pipe(t_token **tokens, t_command **current)
{
	(*current)->next = init_command();
	*current = (*current)->next;
	*tokens = (*tokens)->next;
}

void	handle_word_tokens(t_command	*current, t_token *tokens)
{
	if (!current->cmd_name)
		current->cmd_name = ft_strdup(tokens->value);
	add_arg(&current->argv, tokens->value);
}

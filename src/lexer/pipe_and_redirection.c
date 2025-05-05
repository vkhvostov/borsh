#include "../../include/borsh.h"

t_token	*parse_pipe(int *i)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_PIPE;
	token->value = ft_strdup("|");
	token->next = NULL;
	(*i)++;
	return (token);
}

void	handle_token_redirection(t_token *token, t_token_type type, int *i, char *value)
{
	token->type = type;
	token->value = ft_strdup(value);
	if (type == T_HEREDOC || type == T_REDIR_APPEND)
		(*i) += 2;
	else
		(*i)++;
}

t_token	*parse_redirection(char *input, int *i)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	if (input[*i] == '<' && input[*i + 1] &&  input[*i + 1] == '<')
		handle_token_redirection(token, T_HEREDOC, i, "<<");
	else if (input[*i] == '>' && input[*i + 1] && input[*i + 1] == '>')
		handle_token_redirection(token, T_REDIR_APPEND, i, ">>");
	else if (input[*i] == '<')
		handle_token_redirection(token, T_REDIR_IN, i, "<");
	else if (input[*i] == '>')
		handle_token_redirection(token, T_REDIR_OUT, i, ">");
	token->next = NULL;
	return(token);
}

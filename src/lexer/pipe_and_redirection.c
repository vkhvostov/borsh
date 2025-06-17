#include "../../include/borsh.h"

static t_token	*create_token(void)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->next = NULL;
	return (token);
}

t_token	*parse_pipe(int *i)
{
	t_token	*token;

	token = create_token();
	if (!token)
		return (NULL);
	token->type = T_PIPE;
	token->value = ft_strdup("|");
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	(*i)++;
	return (token);
}

static int	handle_redirection_token(t_token *token, t_token_type type,
	int *i, char *value)
{
	token->type = type;
	token->value = ft_strdup(value);
	if (!token->value)
		return (-1);
	if (type == T_HEREDOC || type == T_REDIR_APPEND)
		(*i) += 2;
	else
		(*i)++;
	return (0);
}

static t_token	*handle_redirection(int *i, t_token_type type, char *value)
{
	t_token	*token;

	token = create_token();
	if (!token)
		return (NULL);
	if (handle_redirection_token(token, type, i, value) == -1)
	{
		free(token);
		return (NULL);
	}
	return (token);
}

t_token	*parse_redirection(char *input, int *i)
{
	if (input[*i] == '<' && input[*i + 1] && input[*i + 1] == '<')
		return (handle_redirection(i, T_HEREDOC, "<<"));
	else if (input[*i] == '>' && input[*i + 1] && input[*i + 1] == '>')
		return (handle_redirection(i, T_REDIR_APPEND, ">>"));
	else if (input[*i] == '<')
		return (handle_redirection(i, T_REDIR_IN, "<"));
	else if (input[*i] == '>')
		return (handle_redirection(i, T_REDIR_OUT, ">"));
	return (NULL);
}

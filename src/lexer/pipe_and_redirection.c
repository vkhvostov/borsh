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

// parses a pipe character '|', creates a T_PIPE token
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

// configures a token for a specific redirection type
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

// creates and configures a token for a redirection
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

t_token	*parse_redirection(char *input, int *i, int *exit_status)
{
	char	c;
	t_token	*token;

	c = input[*i];
	token = NULL;
	if (count_redir_arrows(input, i, exit_status) == -1)
		return (NULL);
	if (c == '<' && input[*i + 1] == '<')
		token = handle_redirection(i, T_HEREDOC, "<<");
	else if (c == '>' && input[*i + 1] == '>')
		token = handle_redirection(i, T_REDIR_APPEND, ">>");
	else if (c == '<')
		token = handle_redirection(i, T_REDIR_IN, "<");
	else
		token = handle_redirection(i, T_REDIR_OUT, ">");
	if (token)
	{
		if (redir_token_check(input, i, exit_status, token) == -1)
			return (NULL);
	}
	return (token);
}

#include "../../include/borsh.h"

t_token	*parse_pipe(int *i)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_PIPE;
	token->value = ft_strdup("|");
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	(*i)++;
	return (token);
}

static int handle_redirection_token(t_token *token, t_token_type type, int *i, char *value)
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

t_token	*parse_redirection(char *input, int *i)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	if (input[*i] == '<' && input[*i + 1] &&  input[*i + 1] == '<')
	{
		if (handle_redirection_token(token, T_HEREDOC, i, "<<") == -1)
		{
			free(token);
			return (NULL);
		}
	}
	else if (input[*i] == '>' && input[*i + 1] && input[*i + 1] == '>')
	{
		if (handle_redirection_token(token, T_REDIR_APPEND, i, ">>") == -1)
		{
			free(token);
			return (NULL);
		}
	}
	else if (input[*i] == '<')
	{
		if (handle_redirection_token(token, T_REDIR_IN, i, "<") == -1)
		{
			free(token);
			return (NULL);
		}
	}
	else if (input[*i] == '>')
	{
		if (handle_redirection_token(token, T_REDIR_OUT, i, ">") == -1)
		{
			free(token);
			return (NULL);
		}
	}
	token->next = NULL;
	return(token);
}

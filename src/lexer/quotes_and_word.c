#include "../../include/borsh.h"

t_token	*parse_single_quote(char *input, int *i)
{
	int		start;
	t_token	*token;

	start = ++(*i);
	while (input[*i] && input[*i] != '\'')
		(*i)++;
	if (input[*i] != '\'')
	{
		printf("Error: Unclosed single quote\n");
		return (NULL);
	}
	token = malloc(sizeof(t_token));
	if(!token)
		return (NULL);
	token->type = T_WORD;
	// !!! WRITE AND ADD FT_STRNDUP TO LIBFT !!!
	token->value = strndup(&input[start], *i - start);
	if (!token->value)
	{
		free(token);
		return NULL;
	}
	token->next = NULL;
	(*i)++;
	return(token);
}

t_token	*parse_double_quote(char *input, int *i)
{
	int	start;
	t_token	*token;

	start = ++(*i);
	while(input[*i] && input[*i] != '"')
		(*i)++;
	if (input[*i] != '"')
	{
		printf("Error: Unclosed double quote\n");
		return (NULL);
	}
	token = malloc(sizeof(t_token));
	if(!token)
		return (NULL);
	token->type = T_WORD;
	// !!! WRITE AND ADD FT_STRNDUP TO LIBFT !!!
	token->value = strndup(&input[start], *i - start);
	if (!token->value)
	{
		free(token);
		return NULL;
	}
	token->next = NULL;
	(*i)++;
	return(token);
}

t_token	*parse_word(char *input, int *i)
{
	int		start;
	t_token	*token;

	start = *i;
	while(input[*i] && input[*i] != ' ' && input[*i] != '|' && input[*i] != '<'
			&& input[*i] != '>' && input[*i] != '"' && input[*i] != '\'')
		(*i)++;
	token = malloc(sizeof(t_token));
	if(!token)
		return (NULL);
	token->type = T_WORD;
	// !!! WRITE AND ADD FT_STRNDUP TO LIBFT !!!
	token->value = strndup(&input[start], *i - start);
	if (!token->value)
	{
		free(token);
		return NULL;
	}
	token->next = NULL;
	return (token);
}

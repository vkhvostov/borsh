#include "../../include/borsh.h"

// creates a token of type T_WORD
t_token	*create_word_token(char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;
	token->value = value;
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

// creates a token for a single-quoted string, type T_WORD
t_token	*create_single_quote_token(char *result)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;
	token->value = result;
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

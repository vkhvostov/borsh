#include "../../include/borsh.h"

t_token	*parse_single_quote(char *input, int *i)
{
	int		start;
	int		quote_start;
	int		quote_end;
	t_token	*token;
	char	*word;
	char	*quoted;
	char	*result;

	start = *i;
	quote_start = ++(*i);
	while (input[*i] && input[*i] != '\'')
		(*i)++;
	if (input[*i] != '\'')
	{
		printf("borsh: syntax error: unclosed single quote\n");
		set_last_exit_status(2);
		return (NULL);
	}
	quote_end = *i;
	(*i)++;
	
	// Continue parsing if there are more word characters
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;

	// If we started with word characters
	if (quote_start > start + 1)
	{
		word = ft_strndup(&input[start], quote_start - start - 1);
		quoted = ft_strndup(&input[quote_start], quote_end - quote_start);
		result = ft_strjoin(word, quoted);
		free(word);
		free(quoted);
	}
	else
		result = ft_strndup(&input[quote_start], quote_end - quote_start);

	// If we have more word characters after the quote
	if (*i > quote_end + 1)
	{
		word = ft_strndup(&input[quote_end + 1], *i - (quote_end + 1));
		quoted = result;
		result = ft_strjoin(quoted, word);
		free(quoted);
		free(word);
	}

	token->value = result;
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

t_token	*parse_double_quote(char *input, int *i)
{
	int		start;
	int		quote_start;
	int		quote_end;
	t_token	*token;
	char	*word;
	char	*quoted;
	char	*result;

	start = *i;
	quote_start = ++(*i);
	while (input[*i] && input[*i] != '"')
		(*i)++;
	if (input[*i] != '"')
	{
		printf("borsh: syntax error: unclosed double quote\n");
		set_last_exit_status(2);
		return (NULL);
	}
	quote_end = *i;
	(*i)++;
	
	// Continue parsing if there are more word characters
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;

	// If we started with word characters
	if (quote_start > start + 1)
	{
		word = ft_strndup(&input[start], quote_start - start - 1);
		quoted = ft_strndup(&input[quote_start], quote_end - quote_start);
		result = ft_strjoin(word, quoted);
		free(word);
		free(quoted);
	}
	else
		result = ft_strndup(&input[quote_start], quote_end - quote_start);

	// If we have more word characters after the quote
	if (*i > quote_end + 1)
	{
		word = ft_strndup(&input[quote_end + 1], *i - (quote_end + 1));
		quoted = result;
		result = ft_strjoin(quoted, word);
		free(quoted);
		free(word);
	}

	token->value = result;
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

t_token	*parse_word(char *input, int *i)
{
	int		start;
	t_token	*token;
	char	*word;
	char	*quoted;
	char	*result;

	start = *i;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;

	// If we hit a quote, we need to handle it
	if (input[*i] && (input[*i] == '\'' || input[*i] == '"'))
	{
		word = ft_strndup(&input[start], *i - start);
		if (input[*i] == '\'')
		{
			token = parse_single_quote(input, i);
			if (!token)
			{
				free(word);
				return (NULL);
			}
		}
		else
		{
			token = parse_double_quote(input, i);
			if (!token)
			{
				free(word);
				return (NULL);
			}
		}
		quoted = token->value;
		result = ft_strjoin(word, quoted);
		free(word);
		free(quoted);
		free(token);

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

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;
	token->value = ft_strndup(&input[start], *i - start);
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

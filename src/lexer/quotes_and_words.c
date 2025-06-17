#include "../../include/borsh.h"

t_token	*parse_single_quote(char *input, int *i)
{
	int		start;
	int		quote_start;
	int		quote_end;
	char	*result;

	start = *i;
	quote_start = ++(*i);
	if (parse_quoted_part_loop(input, i, '\'') == -1)
		return (NULL);
	quote_end = *i;
	(*i)++;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	result = handle_single_quote_content(input, start, quote_start, quote_end);
	result = handle_single_quote_after(input, i, quote_end, result);
	return (create_single_quote_token(result));
}

t_token	*parse_double_quote(char *input, int *i)
{
	t_token	*token;
	char	*result;

	result = NULL;
	result = handle_double_quote_content(input, i, result);
	if (!result)
		return (NULL);
	token = malloc(sizeof(t_token));
	if (!token)
	{
		free(result);
		return (NULL);
	}
	token->type = T_WORD;
	token->value = result;
	token->next = NULL;
	return (token);
}

t_token	*parse_word(char *input, int *i)
{
	int		start;
	char	*word;

	start = *i;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	if (input[*i] && (input[*i] == '\'' || input[*i] == '"'))
	{
		word = handle_word_content(input, start, *i);
		return (handle_word_with_quote(input, i, word));
	}
	return (create_word_token(handle_word_content(input, start, *i)));
}

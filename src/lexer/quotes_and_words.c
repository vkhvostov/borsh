#include "../../include/borsh.h"

// processes the content within a single quote
static char	*process_single_quote(char *input, int *i, int *quote_end,
	int *exit_status)
{
	int		quote_start;

	quote_start = *i;
	if (parse_quoted_part_loop(input, i, input[*i - 1], exit_status) == -1)
		return (NULL);
	*quote_end = *i;
	(*i)++;
	return (handle_quoted_content(input, quote_start, *quote_end));
}

static int	process_word_between(char *input, int *i, char **result)
{
	int		start;
	char	*word;
	char	*temp;

	start = *i;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	if (start == *i)
		return (1);
	word = handle_word_content(input, start, *i);
	if (!word)
		return (0);
	if (!*result)
		*result = word;
	else
	{
		temp = join_word_and_quoted(*result, word);
		if (!temp)
			return (0);
		*result = temp;
	}
	return (1);
}

// updates the result with the quoted content
static char	**update_quoted_content(char **result, char *temp)
{
	if (!*result)
		*result = temp;
	else
	{
		*result = join_word_and_quoted(*result, temp);
		if (!temp || !*result)
			return (NULL);
	}
	return (result);
}

// handles content within the quotes and any adjacent word characters
t_token	*parse_quote(char *input, int *i, int *exit_status)
{
	int		quote_end;
	char	*result;
	char	*temp;

	result = NULL;
	while (input[*i])
	{
		if (input[*i] == '\'' || input[*i] == '"')
		{
			(*i)++;
			temp = process_single_quote(input, i, &quote_end, exit_status);
			if (!update_quoted_content(&result, temp))
				return (NULL);
		}
		else if (is_word_char(input[*i]))
		{
			if (!process_word_between(input, i, &result))
				return (NULL);
		}
		else
			break ;
	}
	return (create_quote_token(result));
}

// parses a word
// if followed by a quote -> `handle_word_with_quote` to combine
t_token	*parse_word(char *input, int *i, int *exit_status)
{
	int		start;
	char	*word;

	start = *i;
	while (input[*i] && (is_word_char(input[*i])
			|| (input[*i] == '\\' && input[*i + 1])))
	{
		if (input[*i] == '\\' && input[*i + 1])
			(*i) += 2;
		else
			(*i)++;
	}
	if (input[*i] && (input[*i] == '\'' || input[*i] == '"'))
	{
		word = handle_word_content(input, start, *i);
		return (handle_word_with_quote(input, i, word, exit_status));
	}
	return (create_word_token(handle_word_content(input, start, *i)));
}

#include "../../include/borsh.h"

// concatenates the part before the quotation marks +
// the content within the quotation marks
// if they are part of a word together
char	*handle_single_quote_content(char *input, int start,
	int quote_start, int quote_end)
{
	char	*word;
	char	*quoted;
	char	*result;

	if (quote_start > start + 1)
	{
		word = handle_word_content(input, start, quote_start - 1);
		quoted = handle_quoted_content(input, quote_start, quote_end);
		result = join_word_and_quoted(word, quoted);
	}
	else
		result = handle_quoted_content(input, quote_start, quote_end);
	return (result);
}

// handles word content that appears after a single-quoted string
char	*handle_single_quote_after(char *input, int *i, int quote_end,
	char *result)
{
	char	*word;
	char	*quoted;

	if (*i > quote_end + 1)
	{
		word = handle_word_content(input, quote_end + 1, *i);
		quoted = result;
		result = join_word_and_quoted(quoted, word);
	}
	return (result);
}

// handles the content within a double-quoted string, which can contain
// other words or quoted sections
char	*handle_double_quote_content(char *input, int *i, char *result,
	int *exit_status)
{
	while (input[*i])
	{
		if (input[*i] == '"')
		{
			result = handle_quoted_part(input, i, result, exit_status);
			if (!result)
				return (NULL);
		}
		else if (is_word_char(input[*i]))
		{
			result = handle_word_part(input, i, result);
			if (!result)
				return (NULL);
		}
		else
			break ;
	}
	return (result);
}

int	handle_single_quotes(char *input, int *i, char **result, int *exit_status)
{
	int		quote_start;
	int		quote_end;
	char	*quoted;

	quote_start = ++(*i);
	if (parse_quoted_part_loop(input, i, '\'', exit_status) == -1)
		return (free(*result), 0);
	quote_end = *i;
	(*i)++;
	quoted = handle_quoted_content(input, quote_start, quote_end);
	if (!quoted)
		return (free(*result), 0);
	*result = handle_quoted_part_result(*result, quoted);
	return (*result != NULL);
}

int	handle_unquoted_word(char *input, int *i, char **result)
{
	int		start;
	char	*word;

	start = *i;
	while (is_word_char(input[*i]) || input[*i] == '\\')
		(*i)++;
	word = parse_unquoted_part(input, start, *i);
	if (!word)
		return (free(*result), 0);
	*result = handle_quoted_part_result(*result, word);
	return (*result != NULL);
}

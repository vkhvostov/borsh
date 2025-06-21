#include "../../include/borsh.h"

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

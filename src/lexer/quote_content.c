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

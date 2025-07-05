#include "../../include/borsh.h"

static int	handle_unclosed_quote(char quote_type, int *exit_status)
{
	if (quote_type == '\'')
		printf("borsh: syntax error: unclosed single quote\n");
	else
		printf("borsh: syntax error: unclosed double quote\n");
	*exit_status = 2;
	return (-1);
}

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

// checks if the quote is closed, handling escaped quotes
int	parse_quoted_part_loop(char *input, int *i, char quote_type,
	int *exit_status)
{
	while (input[*i])
	{
		if (input[*i] == '\\' && input[*i + 1] && input[*i + 1] == quote_type)
			*i += 2;
		else if (input[*i] == quote_type)
			break ;
		else
			(*i)++;
	}
	if (input[*i] != quote_type)
	{
		handle_unclosed_quote(quote_type, exit_status);
		return (-1);
	}
	return (0);
}

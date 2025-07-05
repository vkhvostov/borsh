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

// handles content within the quotes and any adjacent word characters
// creates a single word token
t_token	*parse_quote(char *input, int *i, int *exit_status)
{
	int		quote_start;
	int		quote_end;
	char	*result;
	char	*temp;

	result = NULL;
	while (input[*i] == '\'' || input[*i] == '"')
	{
		quote_start = ++(*i);
		if (input[*i - 1] == '\'')
		{
			if (parse_quoted_part_loop(input, i, '\'', exit_status) == -1)
				return (NULL);
		}
		else
		{
			if (parse_quoted_part_loop(input, i, '"', exit_status) == -1)
				return (NULL);
		}
		quote_end = *i;
		(*i)++;
		temp = handle_quoted_content(input, quote_start, quote_end);
		if (!temp)
			return (NULL);
		if (result)
		{
			result = join_word_and_quoted(result, temp);
			if (!result)
				return (NULL);
		}
		else
			result = temp;
	}
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	result = handle_single_quote_after(input, i, quote_end, result);
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

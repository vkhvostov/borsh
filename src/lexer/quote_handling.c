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

// checks if the quote is closed
int	parse_quoted_part_loop(char *input, int *i, char quote_type,
	int *exit_status)
{
	while (input[*i] && input[*i] != quote_type)
		(*i)++;
	if (input[*i] != quote_type)
	{
		handle_unclosed_quote(quote_type, exit_status);
		return (-1);
	}
	return (0);
}

// extracts the content from within a pair of quotes
char	*handle_quoted_content(char *input, int quote_start, int quote_end)
{
	char	*quoted;

	quoted = ft_strndup(&input[quote_start], quote_end - quote_start);
	if (!quoted)
		return (NULL);
	return (quoted);
}

// joins a newly parsed quoted part with a previous result
char	*handle_quoted_part_result(char *result, char *quoted)
{
	char	*temp;

	if (!result)
		return (quoted);
	temp = result;
	result = join_word_and_quoted(temp, quoted);
	if (!result)
		return (NULL);
	return (result);
}

// handles a complete quoted section
char	*handle_quoted_part(char *input, int *i, char *result, int *exit_status)
{
	int		quote_start;
	int		quote_end;
	char	*quoted;

	quote_start = ++(*i);
	if (parse_quoted_part_loop(input, i, '"', exit_status) == -1)
	{
		if (result)
			free(result);
		return (NULL);
	}
	quote_end = *i;
	(*i)++;
	quoted = handle_quoted_content(input, quote_start, quote_end);
	if (!quoted)
	{
		if (result)
			free(result);
		return (NULL);
	}
	return (handle_quoted_part_result(result, quoted));
}

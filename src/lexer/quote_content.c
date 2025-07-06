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

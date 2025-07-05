#include "../../include/borsh.h"

int	handle_double_quotes(char *input, int *i, char **result, int *exit_status)
{
	*result = handle_quoted_part(input, i, *result, exit_status);
	return (*result != NULL);
}

char	*parse_unquoted_part(char *input, int start, int end)
{
	if (end <= start || !input)
		return (NULL);
	return (ft_substr(input, start, end - start));
}

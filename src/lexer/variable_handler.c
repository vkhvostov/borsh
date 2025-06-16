#include "../../include/borsh.h"

static void	update_quote_state(char c, int *in_single, int *in_double)
{
	if (c == '\'' && !*in_double)
		*in_single = !*in_single;
	else if (c == '"' && !*in_single)
		*in_double = !*in_double;
}

static int	handle_expansion(const char *input, size_t *i, char **result,
	char **env)
{
	if (process_expansion(input, i, result, env) == -1)
	{
		free(*result);
		set_last_exit_status(1);
		return (-1);
	}
	return (0);
}

static int	handle_char(const char *input, size_t *i, char **result)
{
	if (append_chars(input, *i, result) == -1)
	{
		free(*result);
		set_last_exit_status(1);
		return (-1);
	}
	(*i)++;
	return (0);
}

char	*expand_variables(const char *input, char **env)
{
	char	*result;
	size_t	i;
	int		in_single;
	int		in_double;

	result = empty_string();
	if (!result)
		return (NULL);
	i = 0;
	in_single = 0;
	in_double = 0;
	while (input[i])
	{
		update_quote_state(input[i], &in_single, &in_double);
		if (input[i] == '$' && !in_single)
		{
			if (handle_expansion(input, &i, &result, env) == -1)
				return (NULL);
			continue ;
		}
		if (handle_char(input, &i, &result) == -1)
			return (NULL);
	}
	return (result);
}

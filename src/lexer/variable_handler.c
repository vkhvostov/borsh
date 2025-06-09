#include "../../include/borsh.h"

/**
 * This function is called when we encounter a variable
 * in the input string. It extracts the variable name,
 * retrieves its value, and appends it to the result.
 * The input string is assumed to be a valid C string.
 */
static size_t extract_and_expand_var(const char *input, size_t i, char **result)
{
	size_t	var_len;
	char	*variable;
	char	*value;

	var_len = 0;
	while (ft_isalnum(input[i + var_len]) || input[i + var_len] == '_')
		var_len++;
	variable = strndup(&input[i], var_len);
	if (!variable)
	{
		set_last_exit_status(1);
		return (-1);
	}
	value = get_variable_value(variable);
	if (!value)
	{
		free(variable);
		set_last_exit_status(1);
		return (-1);
	}
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(value) + 1);
	if (*result == NULL)
	{
		free(variable);
		free(value);
		set_last_exit_status(1);
		return (-1);
	}
	ft_strlcpy(*result, *result, ft_strlen(*result) + 1);
	ft_strlcat(*result, value, ft_strlen(*result) + ft_strlen(value) + 1);
	free(variable);
	free(value);
	return (var_len);
}

static void update_quote_state(char c, int *in_single, int *in_double)
{
	if (c == '\'' && !*in_double)
		*in_single = !*in_single;
	else if (c == '"' && !*in_single)
		*in_double = !*in_double;
}

static int expand_special_variable(char **result)
{
	char	*value;
	size_t	result_len;
	size_t	value_len;

	value = get_variable_value("?");
	if (!value)
	{
		set_last_exit_status(1);
		return (-1);
	}

	result_len = ft_strlen(*result);
	value_len = ft_strlen(value);
	
	*result = ft_realloc(*result, result_len + value_len + 1);
	if (!*result)
	{
		free(value);
		set_last_exit_status(1);
		return (-1);
	}

	ft_memcpy(*result + result_len, value, value_len + 1);
	free(value);
	return (0);
}

static int process_expansion(const char *input, size_t *i, char **result)
{
	size_t	consumed;
	
	// Check what follows the $
	if (!input[*i + 1] || (input[*i + 1] != '?' && !ft_isalnum(input[*i + 1]) && input[*i + 1] != '_'))
	{
		// If nothing valid follows $, treat it as a literal $
		if (append_chars(input, *i, result) == -1)
		{
			set_last_exit_status(1);
			return (-1);
		}
		(*i)++;
		return (0);
	}

	(*i)++;  // Move past the $
	if (input[*i] == '?')
	{
		if (expand_special_variable(result) == -1)
		{
			set_last_exit_status(1);
			return (-1);
		}
		(*i)++;
		return (0);
	}
	consumed = extract_and_expand_var(input, *i, result);
	if (consumed == (size_t)-1)
	{
		set_last_exit_status(1);
		return (-1);
	}
	*i += consumed;
	return (0);
}

char *expand_variables(const char *input)
{
	char	*result;
	size_t	i;
	int		in_single;
	int		in_double;

	result = empty_string();
	if (!result)
	{
		set_last_exit_status(1);
		return (NULL);
	}
	i = 0;
	in_single = 0;
	in_double = 0;
	while (input[i]) {
		update_quote_state(input[i], &in_single, &in_double);
		if (input[i] == '$' && !in_single)
		{
			if (process_expansion(input, &i, &result) == -1)
			{
				free(result);
				set_last_exit_status(1);
				return (NULL);
			}
			continue ;
		}
		if (append_chars(input, i, &result) == -1)
		{
			free(result);
			set_last_exit_status(1);
			return (NULL);
		}
		i++;
	}
	return (result);
}

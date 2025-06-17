#include "../../include/borsh.h"

static size_t	get_var_len(const char *input, size_t i)
{
	size_t	var_len;

	var_len = 0;
	while (ft_isalnum(input[i + var_len]) || input[i + var_len] == '_')
		var_len++;
	return (var_len);
}

static int	append_var_value(char **result, char *value)
{
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(value) + 1);
	if (*result == NULL)
		return (-1);
	ft_strlcpy(*result, *result, ft_strlen(*result) + 1);
	ft_strlcat(*result, value, ft_strlen(*result) + ft_strlen(value) + 1);
	return (0);
}

static int	expand_var(const char *input, size_t i, char **result, char **env)
{
	size_t	var_len;
	char	*variable;
	char	*value;

	var_len = get_var_len(input, i);
	variable = ft_strndup(&input[i], var_len);
	if (!variable)
		return (-1);
	value = get_variable_value(variable, env);
	if (!value)
	{
		free(variable);
		return (-1);
	}
	if (append_var_value(result, value) == -1)
	{
		free(variable);
		free(value);
		return (-1);
	}
	free(variable);
	free(value);
	return (var_len);
}

static int	expand_special_var(char **result, char **env)
{
	char	*value;
	size_t	result_len;
	size_t	value_len;

	value = get_variable_value("?", env);
	if (!value)
		return (-1);
	result_len = ft_strlen(*result);
	value_len = ft_strlen(value);
	*result = ft_realloc(*result, result_len + value_len + 1);
	if (!*result)
	{
		free(value);
		return (-1);
	}
	ft_memcpy(*result + result_len, value, value_len + 1);
	free(value);
	return (0);
}

int	process_expansion(const char *input, size_t *i, char **result, char **env)
{
	size_t	consumed;

	if (!input[*i + 1] || (input[*i + 1] != '?' && !ft_isalnum(input[*i + 1])
			&& input[*i + 1] != '_'))
	{
		if (append_chars(input, *i, result) == -1)
			return (-1);
		(*i)++;
		return (0);
	}
	(*i)++;
	if (input[*i] == '?')
	{
		if (expand_special_var(result, env) == -1)
			return (-1);
		(*i)++;
		return (0);
	}
	consumed = expand_var(input, *i, result, env);
	if (consumed == (size_t)-1)
		return (-1);
	*i += consumed;
	return (0);
}

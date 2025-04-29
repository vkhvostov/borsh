#include "../include/borsh.h"

static char* empty_string(void)
{
	char *str;

	str = malloc(1);
	if (str == NULL)
		return NULL;
	str[0] = '\0';
	return str;
}

static char *get_var_value(const char *name)
{
	char	*val;

	if (ft_strncmp(name, "?", 2) == 0)
		return ft_itoa(g_exit_status);
	val = getenv(name);
	if (val)
		return ft_strdup(val);
	else
		return ft_strdup("");
}

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
	value = get_var_value(variable);
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(value) + 1);
	if (*result == NULL)
	{
		free(variable);
		free(value);
		return (-1);
	}
	ft_strlcpy(*result, *result, ft_strlen(*result) + 1);
	ft_strlcat(*result, value, ft_strlen(*result) + ft_strlen(value) + 1);
	free(variable);
	free(value);
	return (var_len);
}

static int append_chars(const char *input, size_t i, char **result)
{
	size_t length;

	length = ft_strlen(*result);
	*result = ft_realloc(*result, length + 2);
	if (*result == NULL)
		return (-1);
	(*result)[length] = input[i];
	(*result)[length + 1] = '\0';
	return (0);
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

	value = get_var_value("?");
	if (!value)
		return (-1);
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(value) + 1);
	if (!*result)
	{
		free(value);
		return -1;
	}
	ft_strlcat(*result, value, ft_strlen(*result) + ft_strlen(value) + 1);
	free(value);
	return (0);
}

static int process_variable_expansion(const char *input, size_t *i, char **result)
{
	size_t	consumed;
	
	(*i)++;
	if (input[*i] == '?') {
		if (expand_special_variable(result) == -1)
			return (-1);
		(*i)++;
		return (0);
	}
	consumed = extract_and_expand_var(input, *i, result);
	if (consumed == (size_t)-1)
		return (-1);
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
		return (NULL);
	i = 0;
	in_single = 0;
	in_double = 0;
	while (input[i]) {
		update_quote_state(input[i], &in_single, &in_double);
		if (input[i] == '$' && !in_single)
		{
			if (process_variable_expansion(input, &i, &result) == -1)
				return (NULL);
			continue ;
		}
		if (append_chars(input, i, &result) == -1)
			return (NULL);
		i++;
	}
	return (result);
}

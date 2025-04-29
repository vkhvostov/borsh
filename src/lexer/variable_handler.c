#include "../include/borsh.h"

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
	char	*var;
	char	*val;

	var_len = 0;
	while (ft_isalnum(input[i + var_len]) || input[i + var_len] == '_')
		var_len++;
	var = strndup(&input[i], var_len);
	val = get_var_value(var);
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(val) + 1);
	if (*result == NULL)
	{
		free(var);
		free(val);
		return (-1);
	}
	ft_strlcpy(*result, *result, ft_strlen(*result) + 1);
	ft_strlcat(*result, val, ft_strlen(*result) + ft_strlen(val) + 1);
	free(var);
	free(val);
	return (var_len);
}

char *expand_variables(const char *input)
{
	char *result = malloc(1);
	result[0] = '\0';
	size_t i = 0;
	int in_single = 0;
	int in_double = 0;

	while (input[i]) {
		if (input[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (input[i] == '\"' && !in_single)
			in_double = !in_double;
		else if (input[i] == '$' && !in_single) {
			i++;
			if (input[i] == '?') {
				char *val = get_var_value("?");
				result = ft_realloc(result, strlen(result) + strlen(val) + 1);
				strcat(result, val);
				free(val);
				i++;
				continue;
			}

			i += extract_and_expand_var(input, i, &result);
			continue;
		}

		// Append regular character
		size_t len = strlen(result);
		result = ft_realloc(result, len + 2);
		result[len] = input[i];
		result[len + 1] = '\0';
		i++;
	}

	return result;
}

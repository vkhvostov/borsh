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
			size_t var_len = 0;

			if (input[i] == '?') {
				char *val = get_var_value("?");
				result = realloc(result, strlen(result) + strlen(val) + 1);
				strcat(result, val);
				free(val);
				i++;
				continue;
			}

			while (isalnum(input[i + var_len]) || input[i + var_len] == '_')
				var_len++;

			char *var = strndup(&input[i], var_len);
			char *val = get_var_value(var);
			result = realloc(result, strlen(result) + strlen(val) + 1);
			strcat(result, val);
			free(var);
			free(val);
			i += var_len;
			continue;
		}

		// Append regular character
		size_t len = strlen(result);
		result = realloc(result, len + 2);
		result[len] = input[i];
		result[len + 1] = '\0';
		i++;
	}

	return result;
}

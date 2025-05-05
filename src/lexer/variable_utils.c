#include "../include/borsh.h"

/**
 * This function retrieves the value of a variable from the environment.
 * If the variable is not found, it returns an empty string.
 * If the variable is "?", it returns the exit status as a string.
 */
char *get_variable_value(const char *name)
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
 * This function appends a character to the result string.
 *
 * @param input The input string.
 * @param i The index of the character to append.
 * @param result A pointer to the result string.
 * @return 0 on success, -1 on failure.
 */
int append_chars(const char *input, size_t i, char **result)
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

#include "../../include/borsh.h"

char	*get_variable_value(char *name, char **env, int *exit_status)
{
	int		i;
	int		name_len;
	char	single_digit[2];

	if (ft_strncmp(name, "?", 2) == 0)
		return (ft_itoa(*exit_status));
	if (ft_isdigit(name[0]))
	{
		single_digit[0] = name[0];
		single_digit[1] = '\0';
		if (single_digit[0] == '0')
			return (ft_strdup("borsh"));
		return (ft_strdup(""));
	}
	if (!env)
		return (ft_strdup(""));
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (ft_strdup(env[i] + name_len + 1));
		i++;
	}
	return (ft_strdup(""));
}

int	append_chars(char *input, size_t i, char **result)
{
	size_t	length;

	length = ft_strlen(*result);
	*result = ft_realloc(*result, length + 2);
	if (*result == NULL)
		return (-1);
	(*result)[length] = input[i];
	(*result)[length + 1] = '\0';
	return (0);
}

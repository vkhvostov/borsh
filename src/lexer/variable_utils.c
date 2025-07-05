#include "../../include/borsh.h"

static size_t	get_var_len(const char *input, size_t i)
{
	size_t	var_len;

	var_len = 0;
	while (ft_isalnum(input[i + var_len]) || input[i + var_len] == '_')
		var_len++;
	return (var_len);
}

// appends the value of an expanded variable to the result string
int	append_var_value(char **result, char *value)
{
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(value) + 1);
	if (*result == NULL)
		return (-1);
	ft_strlcpy(*result, *result, ft_strlen(*result) + 1);
	ft_strlcat(*result, value, ft_strlen(*result) + ft_strlen(value) + 1);
	return (0);
}

// expands a regular environment variable ($VAR)
int	expand_var(t_var_ctx *ctx)
{
	size_t	var_len;
	char	*variable;
	char	*value;

	var_len = get_var_len(ctx->input, ctx->i);
	variable = ft_strndup(&ctx->input[ctx->i], var_len);
	if (!variable)
		return (-1);
	value = get_variable_value(variable, ctx->env, ctx->exit_status);
	if (!value)
	{
		free(variable);
		return (-1);
	}
	if (append_var_value(ctx->result, value) == -1)
	{
		free(variable);
		free(value);
		return (-1);
	}
	free(variable);
	free(value);
	return (var_len);
}

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

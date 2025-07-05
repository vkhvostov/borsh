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
static int	append_var_value(char **result, char *value)
{
	*result = ft_realloc(*result, ft_strlen(*result) + ft_strlen(value) + 1);
	if (*result == NULL)
		return (-1);
	ft_strlcpy(*result, *result, ft_strlen(*result) + 1);
	ft_strlcat(*result, value, ft_strlen(*result) + ft_strlen(value) + 1);
	return (0);
}

// expands a regular environment variable ($VAR)
static int	expand_var(t_var_ctx *ctx)
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

// expands the special variable $? to the last exit status
static int	expand_special_var(char **result, char **env, int *exit_status)
{
	char	*value;
	size_t	result_len;
	size_t	value_len;

	value = get_variable_value("?", env, exit_status);
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

// expands a numeric variable ($1, $2, etc)
static int	expand_numeric_var(t_var_ctx *ctx)
{
	char	*variable;
	char	*value;
	size_t	total_len;

	variable = ft_strndup(&ctx->input[ctx->i], 1);
	if (!variable)
		return (-1);
	value = get_variable_value(variable, ctx->env, ctx->exit_status);
	free(variable);
	if (!value)
		return (-1);
	if (append_var_value(ctx->result, value) == -1)
	{
		free(value);
		return (-1);
	}
	free(value);
	total_len = 1;
	while (ft_isdigit(ctx->input[ctx->i + total_len]))
	{
		if (append_chars(ctx->input, ctx->i + total_len, ctx->result) == -1)
			return (-1);
		total_len++;
	}
	return (total_len);
}

static int	should_expand_variable(t_var_ctx *ctx)
{
	if (!ctx->input[ctx->i + 1] || (ctx->input[ctx->i + 1] != '?'
			&& !ft_isalnum(ctx->input[ctx->i + 1])
			&& ctx->input[ctx->i + 1] != '_'))
	{
		if (append_chars(ctx->input, ctx->i, ctx->result) == -1)
			return (-1);
		ctx->i++;
		return (0);
	}
	ctx->i++;
	return (1);
}

// handler for variable expansion
// determines whether to expand a regular variable, a special variable ($?),
// a numeric variable ($1, $2, etc), or treat the '$' as a literal character
int	process_expansion(t_var_ctx *ctx)
{
	size_t	consumed;

	if (should_expand_variable(ctx) == 0)
		return (0);
	if (ctx->input[ctx->i] == '?')
	{
		if (expand_special_var(ctx->result, ctx->env, ctx->exit_status) == -1)
			return (-1);
		ctx->i++;
		return (0);
	}
	if (ft_isdigit(ctx->input[ctx->i]))
	{
		consumed = expand_numeric_var(ctx);
		if (consumed == (size_t)-1)
			return (-1);
		ctx->i += consumed;
		return (0);
	}
	consumed = expand_var(ctx);
	if (consumed == (size_t)-1)
		return (-1);
	ctx->i += consumed;
	return (0);
}

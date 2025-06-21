#include "../../include/borsh.h"

static char	*set_exit_status(int *exit_status)
{
	*exit_status = 1;
	return (NULL);
}

static void	update_quote_state(char c, int *in_single, int *in_double)
{
	if (c == '\'' && !*in_double)
		*in_single = !*in_single;
	else if (c == '"' && !*in_single)
		*in_double = !*in_double;
}

static int	handle_expansion(char *input, char **env, int *exit_status,
	t_var_ctx *ctx)
{
	ctx->input = input;
	ctx->env = env;
	ctx->exit_status = exit_status;
	if (process_expansion(ctx) == -1)
	{
		free(ctx->result);
		return (-1);
	}
	return (0);
}

static int	handle_char(char *input, size_t *i, char **result)
{
	if (append_chars(input, *i, result) == -1)
	{
		free(*result);
		return (-1);
	}
	(*i)++;
	return (0);
}

char	*expand_variables(char *input, char **env, int *exit_status)
{
	char		*result;
	int			in_single;
	int			in_double;
	t_var_ctx	ctx;

	result = empty_string();
	if (!result)
		return (NULL);
	ctx.i = 0;
	ctx.result = &result;
	in_single = 0;
	in_double = 0;
	while (input[ctx.i])
	{
		update_quote_state(input[ctx.i], &in_single, &in_double);
		if (input[ctx.i] == '$' && !in_single)
		{
			if (handle_expansion(input, env, exit_status, &ctx) == -1)
				return (set_exit_status(exit_status));
			continue ;
		}
		if (handle_char(input, &(ctx.i), ctx.result) == -1)
			return (set_exit_status(exit_status));
	}
	return (result);
}

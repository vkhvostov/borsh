#include "../../include/borsh.h"

static bool	resolve_command_path(t_cmd_ctx *ctx, char **original,
	int *exit_status)
{
	*original = ctx->cmd->cmd_name;
	if (*original && (*original)[0])
		ctx->cmd->cmd_name = resolve_path(*original, exit_status);
	if (!ctx->cmd->cmd_name && *original && (*original)[0])
	{
		handle_command_resolution(ctx, *original, exit_status);
		return (false);
	}
	if (*original)
		free(*original);
	return (true);
}

static void	cleanup_on_error(t_cmd_ctx *ctx, char *original, int *exit_status)
{
	*exit_status = 1;
	if (!ctx->is_last)
		cleanup_command_resources(ctx->fds, ctx->pipe_fds);
	else
		cleanup_command_resources(ctx->fds, NULL);
	if (ctx->cmd->cmd_name != original)
	{
		free(ctx->cmd->cmd_name);
		ctx->cmd->cmd_name = original;
	}
}

void	execute_command(t_cmd_ctx *ctx, int *exit_status)
{
	char				*original;
	t_process_params	params;

	if (!resolve_command_path(ctx, &original, exit_status))
		return ;
	prepare_process_params(ctx, &params);
	ctx->pids[ctx->cmd_idx] = launch_process(ctx->cmd, params, exit_status);
	if (ctx->pids[ctx->cmd_idx] == -1)
	{
		cleanup_on_error(ctx, original, exit_status);
		return ;
	}
	close_used_fds(ctx);
}

void	process_command(t_cmd_ctx *ctx, int *exit_status)
{
	if (prepare_command(ctx, exit_status))
		execute_command(ctx, exit_status);
}

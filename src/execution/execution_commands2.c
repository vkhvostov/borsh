#include "../../include/borsh.h"

static bool	resolve_command_path(t_cmd_ctx *ctx, char **original)
{
	*original = ctx->cmd->cmd_name;
	if (*original && (*original)[0])
		ctx->cmd->cmd_name = resolve_path(*original);
	if (!ctx->cmd->cmd_name && *original && (*original)[0])
	{
		handle_command_resolution(ctx, *original);
		return (false);
	}
	return (true);
}

static void	handle_pipe_input(t_cmd_ctx *ctx)
{
	if (*ctx->prev_pipe_read != -1 && ctx->fds[0] == STDIN_FILENO)
	{
		ctx->fds[0] = *ctx->prev_pipe_read;
		*ctx->prev_pipe_read = -1;
	}
}

static void	cleanup_on_error(t_cmd_ctx *ctx, char *original)
{
	set_last_exit_status(1);
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

void	process_command(t_cmd_ctx *ctx)
{
	char				*original;
	t_process_params	params;
	bool				skip;

	setup_command_io(ctx, &skip);
	if (skip)
		return (handle_skipped_command(ctx));
	handle_pipe_input(ctx);
	if (is_builtin(ctx->cmd))
		return (handle_builtin_command(ctx));
	if (!resolve_command_path(ctx, &original))
		return ;
	prepare_process_params(ctx, &params);
	ctx->pids[ctx->cmd_idx] = launch_process(ctx->cmd, params);
	if (ctx->pids[ctx->cmd_idx] == -1)
	{
		cleanup_on_error(ctx, original);
		return ;
	}
	close_used_fds(ctx);
}

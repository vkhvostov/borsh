#include "../../include/borsh.h"

static void	handle_pipe_input(t_cmd_ctx *ctx)
{
	if (*ctx->prev_pipe_read != -1 && ctx->fds[0] == STDIN_FILENO)
	{
		ctx->fds[0] = *ctx->prev_pipe_read;
		*ctx->prev_pipe_read = -1;
	}
}

bool	prepare_command(t_cmd_ctx *ctx, int *exit_status)
{
	bool	skip;

	if (!ctx->cmd->cmd_name)
		return (handle_skipped_command(ctx, exit_status), false);
	setup_command_io(ctx, &skip, exit_status);
	if (skip)
		return (handle_skipped_command(ctx, exit_status), false);
	handle_pipe_input(ctx);
	if (is_builtin(ctx->cmd))
		return (handle_builtin_command(ctx, exit_status), false);
	if (strcmp(ctx->cmd->cmd_name, "..") == 0)
	{
		ft_putstr_fd("borsh: ..: command not found\n", STDERR_FILENO);
		*exit_status = 127;
		return (false);
	}
	return (true);
}

void	free_and_exit(t_cmd_ctx *ctx, int exit_status)
{
	if (!ctx)
		return ;
	safe_close(ctx->fds[0]);
	safe_close(ctx->fds[1]);
	safe_close(ctx->pipe_fds[0]);
	safe_close(ctx->pipe_fds[1]);
	// free_commands(ctx->cmd);
	// free_str_array(ctx->env);
	// free(ctx);
	exit(exit_status);
}
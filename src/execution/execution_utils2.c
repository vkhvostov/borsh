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

	if (!ctx->cmd->cmd_name || ctx->cmd->cmd_name[0] == '\0')
	{
		ft_putstr_fd("borsh: : command not found\n", STDERR_FILENO);
		*exit_status = 127;
		return (handle_skipped_command(ctx, exit_status), false);
	}
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
	free_commands(ctx->cmd);
	free_shell_env(*ctx->env);
	free(ctx->pids);
	exit(exit_status);
}

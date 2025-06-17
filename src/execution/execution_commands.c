#include "../../include/borsh.h"

void	setup_command_io(t_cmd_ctx *ctx, bool *should_skip_command)
{
	ctx->fds[0] = STDIN_FILENO;
	ctx->fds[1] = STDOUT_FILENO;
	*should_skip_command = false;
	if (!ctx->is_last && pipe(ctx->pipe_fds) == -1)
	{
		ft_putstr_fd("pipe failed\n", STDERR_FILENO);
		set_last_exit_status(1);
		*should_skip_command = true;
		return ;
	}
	if (handle_redirections(ctx->cmd, &ctx->fds[0], &ctx->fds[1]) == -1)
	{
		set_last_exit_status(1);
		*should_skip_command = true;
		if (!ctx->is_last)
		{
			safe_close(ctx->pipe_fds[0]);
			safe_close(ctx->pipe_fds[1]);
		}
	}
}

void	handle_skipped_command(t_cmd_ctx *ctx)
{
	if (*ctx->prev_pipe_read != -1)
	{
		close(*ctx->prev_pipe_read);
		*ctx->prev_pipe_read = -1;
	}
	if (!ctx->is_last)
	{
		if (pipe(ctx->pipe_fds) == -1)
		{
			perror("pipe failed");
			set_last_exit_status(1);
			return ;
		}
		close(ctx->pipe_fds[1]);
		*ctx->prev_pipe_read = ctx->pipe_fds[0];
	}
	ctx->pids[ctx->cmd_idx] = -1;
}

void	handle_command_resolution(t_cmd_ctx *ctx, char *original)
{
	if (errno == EISDIR)
		fprintf(stderr, "borsh: %s: is a directory\n", original);
	else if (errno == EACCES)
		fprintf(stderr, "borsh: %s: Permission denied\n", original);
	else if (errno == ENOENT && strchr(original, '/') != NULL)
		fprintf(stderr, "borsh: %s: No such file or directory\n", original);
	else
		fprintf(stderr, "borsh: %s: command not found\n", original);
	if (errno == EISDIR || errno == EACCES)
		set_last_exit_status(126);
	else
		set_last_exit_status(127);
	ctx->cmd->cmd_name = original;
	if (!ctx->is_last)
		cleanup_command_resources(ctx->fds, ctx->pipe_fds);
	else
		cleanup_command_resources(ctx->fds, NULL);
}

void	prepare_process_params(t_cmd_ctx *ctx, t_process_params *params)
{
	params->in_fd = ctx->fds[0];
	params->out_fd = ctx->fds[1];
	if (!ctx->is_last)
	{
		params->pipe_fds[0] = ctx->pipe_fds[0];
		params->pipe_fds[1] = ctx->pipe_fds[1];
	}
	else
	{
		params->pipe_fds[0] = -1;
		params->pipe_fds[1] = -1;
	}
	params->is_last_command = ctx->is_last;
	params->env = ctx->env;
}

void	close_used_fds(t_cmd_ctx *ctx)
{
	if (ctx->fds[0] != STDIN_FILENO)
		close(ctx->fds[0]);
	if (ctx->fds[1] != STDOUT_FILENO)
		close(ctx->fds[1]);
	if (!ctx->is_last)
	{
		close(ctx->pipe_fds[1]);
		*ctx->prev_pipe_read = ctx->pipe_fds[0];
	}
}

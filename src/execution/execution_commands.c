#include "../../include/borsh.h"

void	setup_command_io(t_cmd_ctx *ctx, bool *should_skip_command,
	int *exit_status)
{
	ctx->fds[0] = STDIN_FILENO;
	ctx->fds[1] = STDOUT_FILENO;
	*should_skip_command = false;
	if (!ctx->is_last && pipe(ctx->pipe_fds) == -1)
	{
		ft_putstr_fd("pipe failed\n", STDERR_FILENO);
		*exit_status = 1;
		*should_skip_command = true;
		return ;
	}
	if (handle_redirections(ctx->cmd, &ctx->fds[0], &ctx->fds[1],
			exit_status) == -1)
	{
		*exit_status = 1;
		*should_skip_command = true;
		if (!ctx->is_last)
		{
			safe_close(ctx->pipe_fds[0]);
			safe_close(ctx->pipe_fds[1]);
		}
	}
}

void	handle_skipped_command(t_cmd_ctx *ctx, int *exit_status)
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
			*exit_status = 1;
			return ;
		}
		close(ctx->pipe_fds[1]);
		*ctx->prev_pipe_read = ctx->pipe_fds[0];
	}
	ctx->pids[ctx->cmd_idx] = -1;
}

void	handle_command_resolution(t_cmd_ctx *ctx, char *original,
	int *exit_status)
{
	ft_putstr_fd("borsh: ", STDERR_FILENO);
	ft_putstr_fd(original, STDERR_FILENO);
	if (errno == EISDIR)
		ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
	else if (errno == EACCES)
		ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
	else if (errno == ENOENT && strchr(original, '/') != NULL)
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
	else
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
	if (errno == EISDIR || errno == EACCES)
		*exit_status = 126;
	else
		*exit_status = 127;
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

#include "../../include/borsh.h"

static bool	is_env_modifier(t_command *cmd)
{
	if (ft_strcmp(cmd->cmd_name, "cd") == 0)
		return (true);
	if (ft_strcmp(cmd->cmd_name, "export") == 0)
		return (true);
	if (ft_strcmp(cmd->cmd_name, "unset") == 0)
		return (true);
	if (ft_strcmp(cmd->cmd_name, "exit") == 0)
		return (true);
	return (false);
}

static void	execute_env_builtin(t_cmd_ctx *ctx, int *exit_status)
{
	int	status;

	status = 0;
	if (ft_strcmp(ctx->cmd->cmd_name, "cd") == 0)
		status = builtin_cd(ctx->cmd->argv, ctx->env);
	else if (ft_strcmp(ctx->cmd->cmd_name, "export") == 0)
		status = builtin_export(ctx->cmd->argv, ctx->env);
	else if (ft_strcmp(ctx->cmd->cmd_name, "unset") == 0)
		status = builtin_unset(ctx->cmd->argv, ctx->env);
	else if (ft_strcmp(ctx->cmd->cmd_name, "exit") == 0)
		status = builtin_exit(ctx->cmd->argv, exit_status);
	*exit_status = status;
	if (!ctx->is_last)
		cleanup_command_resources(ctx->fds, ctx->pipe_fds);
	else
		cleanup_command_resources(ctx->fds, NULL);
	if (ft_strcmp(ctx->cmd->cmd_name, "exit") == 0)
		free_and_exit(ctx, status);
}

static void	exec_builtin_child(t_cmd_ctx *ctx, int *exit_status)
{
	int	status;

	if (ctx->fds[0] != STDIN_FILENO)
		dup2(ctx->fds[0], STDIN_FILENO);
	if (ctx->fds[1] != STDOUT_FILENO)
		dup2(ctx->fds[1], STDOUT_FILENO);
	else if (!ctx->is_last)
		dup2(ctx->pipe_fds[1], STDOUT_FILENO);
	if (ctx->fds[0] != STDIN_FILENO)
		close(ctx->fds[0]);
	if (ctx->fds[1] != STDOUT_FILENO)
		close(ctx->fds[1]);
	if (!ctx->is_last)
	{
		close(ctx->pipe_fds[0]);
		close(ctx->pipe_fds[1]);
	}
	status = execute_builtin(ctx->cmd, ctx->env, exit_status);
	free_and_exit(ctx, status);
}

static void	execute_builtin_command(t_cmd_ctx *ctx, int *exit_status)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		*exit_status = 1;
		return ;
	}
	if (pid == 0)
		exec_builtin_child(ctx, exit_status);
	ctx->pids[ctx->cmd_idx] = pid;
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

void	handle_builtin_command(t_cmd_ctx *ctx, int *exit_status)
{
	if (ctx->cmd_idx == 0 && ctx->cmd->next == NULL
		&& is_env_modifier(ctx->cmd))
	{
		execute_env_builtin(ctx, exit_status);
		return ;
	}
	execute_builtin_command(ctx, exit_status);
}

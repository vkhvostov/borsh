#include "../../include/borsh.h"


// Helper to count commands
static int count_commands(t_command *commands) {
	int count = 0;
	t_command *current = commands;
	while (current != NULL) {
		count++;
		current = current->next;
	}
	return count;
}

// Helper to close FDs, ensuring not to close standard ones unless intended
static void safe_close(int fd) {
	if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO && fd != -1) {
		close(fd);
	}
}

static void cleanup_command_resources(int *fds, int *pipe_fds)
{
	safe_close(fds[0]);
	safe_close(fds[1]);
	if (pipe_fds)
	{
		safe_close(pipe_fds[0]);
		safe_close(pipe_fds[1]);
	}
}

static void	handle_signal_status(int status)
{
	if (WTERMSIG(status) == SIGINT)
	{
		set_last_exit_status(130);
		ft_putstr_fd("\n", STDERR_FILENO);
	}
	else if (WTERMSIG(status) == SIGQUIT)
	{
		set_last_exit_status(131);
		ft_putstr_fd("Quit: 3\n", STDERR_FILENO);
	}
	else
		set_last_exit_status(128 + WTERMSIG(status));
}

static void	handle_child_exit_status(int status)
{
	if (WIFEXITED(status))
		set_last_exit_status(WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		handle_signal_status(status);
}

static void	wait_for_child(pid_t pid, int is_last)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		ft_putstr_fd("waitpid failed\n", STDERR_FILENO);
		if (is_last)
			set_last_exit_status(1);
	}
	else if (is_last)
		handle_child_exit_status(status);
}

static void	wait_for_children(pid_t *pids, int cmd_idx)
{
	int	i;
	int	last_cmd_idx;

	last_cmd_idx = cmd_idx - 1;
	i = 0;
	while (i < cmd_idx)
	{
		if (pids[i] > 0)
			wait_for_child(pids[i], i == last_cmd_idx);
		i++;
	}
}

static void	setup_command_io(t_command *cmd, int *fds, int *pipe_fds, bool is_last, bool *should_skip_command)
{
	fds[0] = STDIN_FILENO;
	fds[1] = STDOUT_FILENO;
	*should_skip_command = false;
	
	if (!is_last && pipe(pipe_fds) == -1)
	{
		ft_putstr_fd("pipe failed\n", STDERR_FILENO);
		set_last_exit_status(1);
		*should_skip_command = true;
		return;
	}
	if (handle_redirections(cmd, &fds[0], &fds[1]) == -1)
	{
		set_last_exit_status(1);
		*should_skip_command = true;
		if (!is_last)
		{
			safe_close(pipe_fds[0]);
			safe_close(pipe_fds[1]);
		}
	}
}

static bool	is_env_modifier(t_command *cmd)
{
	return (ft_strcmp(cmd->cmd_name, "cd") == 0
		|| ft_strcmp(cmd->cmd_name, "export") == 0
		|| ft_strcmp(cmd->cmd_name, "unset") == 0
		|| ft_strcmp(cmd->cmd_name, "exit") == 0);
}

static void	execute_env_builtin(t_command *cmd, char ***env, int is_last,
	int *fds, int *pipe_fds)
{
	int	status;

	status = 0;
	if (ft_strcmp(cmd->cmd_name, "cd") == 0)
		status = builtin_cd(cmd->argv);
	else if (ft_strcmp(cmd->cmd_name, "export") == 0)
		status = builtin_export(cmd->argv, env);
	else if (ft_strcmp(cmd->cmd_name, "unset") == 0)
		status = builtin_unset(cmd->argv, env);
	else if (ft_strcmp(cmd->cmd_name, "exit") == 0)
		status = builtin_exit(cmd->argv);
	set_last_exit_status(status);
	cleanup_command_resources(fds, !is_last ? pipe_fds : NULL);
}

static void	exec_builtin_child(t_command *cmd, int *fds, int *pipe_fds,
	bool is_last, char ***env)
{
	if (fds[0] != STDIN_FILENO)
		dup2(fds[0], STDIN_FILENO);
	if (fds[1] != STDOUT_FILENO)
		dup2(fds[1], STDOUT_FILENO);
	else if (!is_last)
		dup2(pipe_fds[1], STDOUT_FILENO);
	if (fds[0] != STDIN_FILENO)
		close(fds[0]);
	if (fds[1] != STDOUT_FILENO)
		close(fds[1]);
	if (!is_last)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
	}
	exit(execute_builtin(cmd, env));
}

static void	execute_builtin_command(t_command *cmd, pid_t *pids,
	int cmd_idx, int *prev_pipe_read, int *fds,
	int *pipe_fds, bool is_last, char ***env)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		set_last_exit_status(1);
		return ;
	}
	if (pid == 0)
		exec_builtin_child(cmd, fds, pipe_fds, is_last, env);
	pids[cmd_idx] = pid;
	if (fds[0] != STDIN_FILENO)
		close(fds[0]);
	if (fds[1] != STDOUT_FILENO)
		close(fds[1]);
	if (!is_last)
	{
		close(pipe_fds[1]);
		*prev_pipe_read = pipe_fds[0];
	}
}

static void	handle_skipped_command(int *prev_pipe_read, int *pipe_fds,
	pid_t *pids, int cmd_idx, bool is_last)
{
	if (*prev_pipe_read != -1)
	{
		close(*prev_pipe_read);
		*prev_pipe_read = -1;
	}
	if (!is_last)
	{
		if (pipe(pipe_fds) == -1)
		{
			perror("pipe failed");
			set_last_exit_status(1);
			return ;
		}
		close(pipe_fds[1]);
		*prev_pipe_read = pipe_fds[0];
	}
	pids[cmd_idx] = -1;
}

static void handle_command_resolution(t_cmd_ctx *ctx, char *original)
{
	if (errno == EISDIR)
		fprintf(stderr, "borsh: %s: is a directory\n", original);
	else if (errno == EACCES)
		fprintf(stderr, "borsh: %s: Permission denied\n", original);
	else if (errno == ENOENT && strchr(original, '/') != NULL)
		fprintf(stderr, "borsh: %s: No such file or directory\n", original);
	else
		fprintf(stderr, "borsh: %s: command not found\n", original);
	set_last_exit_status(errno == EISDIR || errno == EACCES ? 126 : 127);
	ctx->cmd->cmd_name = original;
	cleanup_command_resources(ctx->fds, !ctx->is_last ? ctx->pipe_fds : NULL);
}

static void prepare_process_params(t_cmd_ctx *ctx, t_process_params *params)
{
	params->in_fd = ctx->fds[0];
	params->out_fd = ctx->fds[1];
	params->pipe_fds[0] = !ctx->is_last ? ctx->pipe_fds[0] : -1;
	params->pipe_fds[1] = !ctx->is_last ? ctx->pipe_fds[1] : -1;
	params->is_last_command = ctx->is_last;
	params->env = ctx->env;
}

static void close_used_fds(t_cmd_ctx *ctx)
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

static void	handle_builtin_command(t_cmd_ctx *ctx)
{
	if (ctx->cmd_idx == 0 && ctx->cmd->next == NULL &&
		is_env_modifier(ctx->cmd))
	{
		execute_env_builtin(ctx->cmd, ctx->env, ctx->is_last,
			ctx->fds, ctx->pipe_fds);
		return ;
	}
	execute_builtin_command(ctx->cmd, ctx->pids, ctx->cmd_idx,
		ctx->prev_pipe_read, ctx->fds, ctx->pipe_fds, ctx->is_last, ctx->env);
}

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
	cleanup_command_resources(ctx->fds,
		!ctx->is_last ? ctx->pipe_fds : NULL);
	if (ctx->cmd->cmd_name != original)
	{
		free(ctx->cmd->cmd_name);
		ctx->cmd->cmd_name = original;
	}
}

static void	process_command(t_cmd_ctx *ctx)
{
	char				*original;
	t_process_params	params;
	bool				skip;

	setup_command_io(ctx->cmd, ctx->fds, ctx->pipe_fds, ctx->is_last, &skip);
	if (skip)
		return (handle_skipped_command(ctx->prev_pipe_read, ctx->pipe_fds,
			ctx->pids, ctx->cmd_idx, ctx->is_last));
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

static bool	init_pids(pid_t **pids, int num_cmds)
{
	*pids = malloc(num_cmds * sizeof(pid_t));
	if (*pids == NULL)
	{
		perror("malloc for pids failed");
		set_last_exit_status(1);
		return false;
	}
	for (int i = 0; i < num_cmds; ++i)
		(*pids)[i] = -1;
	return true;
}

static void	execute_commands(t_command *commands, t_cmd_ctx *ctx,
								bool *command_executed)
{
	t_command	*current_cmd = commands;
	int			cmd_idx = 0;
	int			fds[2];
	int			pipe_fds[2];

	while (current_cmd != NULL)
	{
		ctx->cmd = current_cmd;
		ctx->cmd_idx = cmd_idx;
		ctx->is_last = current_cmd->next == NULL;
		ctx->fds[0] = fds[0];
		ctx->fds[1] = fds[1];
		ctx->pipe_fds[0] = pipe_fds[0];
		ctx->pipe_fds[1] = pipe_fds[1];
		process_command(ctx);
		if (ctx->pids[ctx->cmd_idx] != -1)
			*command_executed = true;
		current_cmd = current_cmd->next;
		cmd_idx++;
	}
}

void execute(t_command *commands, char ***env)
{
	t_cmd_ctx	ctx;
	pid_t		*pids = NULL;
	int			num_cmds;
	int			prev_pipe_read = -1;
	bool		command_executed = false;

	if (commands == NULL)
		return ;
	num_cmds = count_commands(commands);
	if (num_cmds == 0)
		return ;
	if (!init_pids(&pids, num_cmds))
		return ;
	ctx.pids = pids;
	ctx.prev_pipe_read = &prev_pipe_read;
	ctx.env = env;
	execute_commands(commands, &ctx, &command_executed);
	safe_close(prev_pipe_read);
	if (command_executed)
		wait_for_children(pids, num_cmds);

	free(pids);
}

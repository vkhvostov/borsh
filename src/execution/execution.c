#include "../../include/borsh.h"

static bool	init_pids(pid_t **pids, int num_cmds)
{
	int	i;

	*pids = malloc(num_cmds * sizeof(pid_t));
	if (*pids == NULL)
	{
		perror("malloc for pids failed");
		set_last_exit_status(1);
		return (false);
	}
	i = 0;
	while (i < num_cmds)
	{
		(*pids)[i] = -1;
		i++;
	}
	return (true);
}

static void	execute_commands(t_command *commands, t_cmd_ctx *ctx,
	bool *command_executed)
{
	t_command	*current_cmd;
	int			cmd_idx;
	int			fds[2];
	int			pipe_fds[2];

	current_cmd = commands;
	cmd_idx = 0;
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

void	execute(t_command *commands, char ***env)
{
	t_cmd_ctx	ctx;
	pid_t		*pids;
	int			num_cmds;
	int			prev_pipe_read;
	bool		command_executed;

	if (commands == NULL)
		return ;
	num_cmds = count_commands(commands);
	if (num_cmds == 0)
		return ;
	if (!init_pids(&pids, num_cmds))
		return ;
	prev_pipe_read = -1;
	command_executed = false;
	ctx.pids = pids;
	ctx.prev_pipe_read = &prev_pipe_read;
	ctx.env = env;
	execute_commands(commands, &ctx, &command_executed);
	safe_close(prev_pipe_read);
	if (command_executed)
		wait_for_children(pids, num_cmds);
	free(pids);
}

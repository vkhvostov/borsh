#include "../../include/borsh.h"

static bool	init_pids(pid_t **pids, int num_cmds, int *exit_status)
{
	int	i;

	*pids = malloc(num_cmds * sizeof(pid_t));
	if (*pids == NULL)
	{
		perror("malloc for pids failed");
		*exit_status = 1;
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
	bool *command_executed, int *exit_status)
{
	t_command	*current_cmd;
	int			cmd_idx;

	current_cmd = commands;
	cmd_idx = 0;
	while (current_cmd != NULL)
	{
		ctx->cmd = current_cmd;
		ctx->cmd_idx = cmd_idx;
		ctx->is_last = current_cmd->next == NULL;
		process_command(ctx, exit_status);
		if (ctx->pids[ctx->cmd_idx] != -1)
			*command_executed = true;
		current_cmd = current_cmd->next;
		cmd_idx++;
	}
}

void	execute(t_command *commands, char ***env, int *exit_status)
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
	if (!init_pids(&pids, num_cmds, exit_status))
		return ;
	prev_pipe_read = -1;
	command_executed = false;
	ctx.pids = pids;
	ctx.prev_pipe_read = &prev_pipe_read;
	ctx.env = env;
	execute_commands(commands, &ctx, &command_executed, exit_status);
	safe_close(prev_pipe_read);
	if (command_executed)
		wait_for_children(pids, num_cmds, exit_status);
	free(pids);
}

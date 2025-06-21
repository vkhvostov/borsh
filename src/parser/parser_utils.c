#include "../../include/borsh.h"

void	memory_error_handler(int *exit_status)
{
	*exit_status = 1;
	return ;
}

t_command	*init_command(char **env, int *exit_status)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
	{
		*exit_status = 1;
		return (NULL);
	}
	cmd->cmd_name = NULL;
	cmd->argv = NULL;
	cmd->env = copy_environment(env, exit_status);
	if (!cmd->env)
	{
		free(cmd);
		*exit_status = 1;
		return (NULL);
	}
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

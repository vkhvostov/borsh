#include "../../include/borsh.h"

t_command	*init_command(char **env)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
	{
		set_last_exit_status(1);
		return (NULL);
	}
	cmd->cmd_name = NULL;
	cmd->argv = NULL;
	cmd->env = copy_environment(env);
	if (!cmd->env)
	{
		free(cmd);
		set_last_exit_status(1);
		return (NULL);
	}
	cmd->redirs = NULL;
	cmd->next = NULL;
	return (cmd);
}

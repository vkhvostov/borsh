#include "../../include/borsh.h"

void	memory_error_handler(int *exit_status)
{
	*exit_status = 1;
	return ;
}

t_command	*exit_on_error(t_command *cmd_list, int *exit_status, int status)
{
	free_commands(cmd_list);
	*exit_status = status;
	return (NULL);
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

int	handle_word_token_logic(t_command *cmd, t_token *token, int *status)
{
	if (!handle_word_tokens(cmd, token, status))
	{
		*status = 1;
		return (0);
	}
	return (1);
}

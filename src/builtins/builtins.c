#include "../../include/borsh.h"

int	is_builtin(t_command *cmd)
{
	if (!cmd)
		return (0);
	return (
		ft_strcmp(cmd->cmd_name, "echo") == 0 ||
		ft_strcmp(cmd->cmd_name, "cd") == 0 ||
		ft_strcmp(cmd->cmd_name, "pwd") == 0 ||
		ft_strcmp(cmd->cmd_name, "export") == 0 ||
		ft_strcmp(cmd->cmd_name, "unset") == 0 ||
		ft_strcmp(cmd->cmd_name, "env") == 0 ||
		ft_strcmp(cmd->cmd_name, "exit") == 0
	);
}
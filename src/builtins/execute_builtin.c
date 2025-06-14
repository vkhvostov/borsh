#include "../../include/borsh.h"

int	execute_builtin(t_command *cmd, char ***env)
{
	if (!cmd || !cmd->cmd_name)
		return (1);
	if (strcmp(cmd->cmd_name, "echo") == 0)
		return (builtin_echo(cmd->argv));
	else if (strcmp(cmd->cmd_name, "cd") == 0)
		return (builtin_cd(cmd->argv));
	else if (strcmp(cmd->cmd_name, "pwd") == 0)
		return (builtin_pwd(cmd->argv));
	else if (strcmp(cmd->cmd_name, "export") == 0)
		return (builtin_export(cmd->argv, env));
	else if (strcmp(cmd->cmd_name, "unset") == 0)
		return (builtin_unset(cmd->argv, env));
	else if (strcmp(cmd->cmd_name, "env") == 0)
		return (builtin_env(cmd->argv, env));
	else if (strcmp(cmd->cmd_name, "exit") == 0)
		(builtin_exit(cmd->argv));
	return (1);
}
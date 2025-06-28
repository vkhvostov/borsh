#include "../../include/borsh.h"

// prints the current working directory to stdout
// 0 on success, 1 on error
int	builtin_pwd(void)
{
	char	*cmd;

	cmd = get_pwd();
	if (cmd == NULL)
	{
		ft_putstr_fd("pwd: error retrieving current directory\n",
			STDERR_FILENO);
		return (1);
	}
	printf("%s\n", cmd);
	free(cmd);
	return (0);
}

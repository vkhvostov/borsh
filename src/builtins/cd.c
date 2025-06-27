#include "../../include/borsh.h"

// changes the current working directory to the specified path.
int	builtin_cd(char **argv)
{
	char	*target;

	if (argv[2] != NULL)
	{
		ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (argv[1] == NULL || (ft_strcmp(argv[1], "~") == 0))
		target = getenv("HOME");
	else
		target = argv[1];
	if (target == NULL)
	{
		ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (1);
	}
	if (chdir(target) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

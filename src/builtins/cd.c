#include "../../include/borsh.h"

int	builtin_cd(char **argv)
{
	char	*target;

	if (argv[1] == NULL || (ft_strcmp(argv[1], "~") == 0))
		target = getenv("HOME");
	else
		target = argv[1];
	if (target == NULL)
	{
		write(2, "cd: HOME not set\n", 17);
		return (1);
	}
	if (chdir(target) != 0)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

#include "../../include/borsh.h"

// prints all environment variables to stdout
int	builtin_env(char **argv, char ***env)
{
	int	i;

	(void)argv;
	if (!env || !*env)
		return (1);
	i = 0;
	while ((*env)[i])
	{
		printf("%s\n", (*env)[i]);
		i++;
	}
	return (0);
}

#include "../../include/borsh.h"

int	builtin_export(char **argv, char **env)
{
	int	i;

	i = 0;
	if (argv[1] != NULL)
	{
		write(2, "export: no arguments allowed (yet)\n", 35);
		return (1);
	}
	while(env[i])
	{
		printf("declare -x %s\n", env[i]);
		i++;
	}
	return(0);
}
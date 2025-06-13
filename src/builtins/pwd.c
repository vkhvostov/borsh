#include "../../include/borsh.h"

int	builtin_pwd(char **argv)
{
	char	cwd[1024];

	if (argv[1] != NULL)
	{
		write(2, "pwd: too many arguments\n", 25);
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return(0);
	}
	write(2, "pwd: error retrieving current directory\n", 41);
	return (1);	
}
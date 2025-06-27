#include "../../include/borsh.h"

// prints the current working directory to stdout
// 0 on success, 1 on error
int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	write(2, "pwd: error retrieving current directory\n", 41);
	return (1);
}

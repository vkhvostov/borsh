#include "../../include/borsh.h"

// retrieves the current working directory and sets the PWD environment variable
static int	set_pwd_var(char ***env)
{
	char	*cwd;

	cwd = get_pwd();
	if (cwd == NULL)
	{
		ft_putstr_fd("cd: error retrieving current directory\n", STDERR_FILENO);
		return (1);
	}
	if (set_env_var(env, "PWD", cwd) != 0)
	{
		ft_putstr_fd("cd: failed to set PWD variable\n", STDERR_FILENO);
		free(cwd);
		return (1);
	}
	free(cwd);
	return (0);
}

// changes the current working directory to the specified path.
int	builtin_cd(char **argv, char ***env)
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
	if (set_pwd_var(env) != 0)
		return (1);
	return (0);
}

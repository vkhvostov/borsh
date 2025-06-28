#include "../../include/borsh.h"

static bool	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (!str || !str[0])
		return (false);
	if (str[0] == '-' || str[0] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (false);
		i++;
	}
	return (true);
}

// exits the shell with the given status code
int	builtin_exit(char **argv, int *exit_status)
{
	int	status;

	status = 0;
	// ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ft_putstr_fd("exit: numeric argument required\n", STDERR_FILENO);
			*exit_status = 2;
			exit(2);
		}
		if (argv[2])
		{
			ft_putstr_fd("exit: too many arguments\n", STDERR_FILENO);
			*exit_status = 1;
			return (1);
		}
		status = ft_atoi(argv[1]);
	}
	exit(status);
}

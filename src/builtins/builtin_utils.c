#include "../../include/borsh.h"

static bool	is_numeric(const char *str)
{
	int i = 0;

	if (!str || !str[0])
		return false;
	if (str[0] == '-' || str[0] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return false;
		i++;
	}
	return true;
}

int	builtin_exit(char **argv)
{
	int status = 0;

	write(1, "exit\n", 5);

	if (argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			write(2, "exit: numeric argument required\n", 32);
			exit(255);
			return (1);
		}
		if (argv[2])
		{
			write(2, "exit: too many arguments\n", 26);
			// Do NOT exit here! Return to shell instead.
			set_last_exit_status(1);
			return (1);
		}
		status = ft_atoi(argv[1]);
	}
	exit(status);
	return (0);
}
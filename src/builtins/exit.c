#include "../../include/borsh.h"

static long long	ft_atoll(const char *str)
{
	int			index;
	long long	result;
	int			sign;

	index = 0;
	result = 0;
	sign = 1;
	while (str[index] == ' ' || (str[index] >= 9 && str[index] <= 13))
		index++;
	if (str[index] == '-' || str[index] == '+')
	{
		if (str[index] == '-')
			sign = -1;
		index++;
	}
	while (str[index] >= '0' && str[index] <= '9')
	{
		result = result * 10 + str[index] - '0';
		index++;
	}
	return (result * sign);
}

static bool	is_numeric(const char *str)
{
	int	i;

	if (ft_strlen(str) >= 20)
		return (false);
	if (ft_atoll(str) > LONG_MAX || ft_atoll(str) < LONG_MIN)
		return (false);
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
	ft_putstr_fd("exit\n", STDOUT_FILENO);
	if (argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ft_putstr_fd("borsh: exit: ", STDERR_FILENO);
			ft_putstr_fd(argv[1], STDERR_FILENO);
			ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
			*exit_status = 2;
			return (2);
		}
		if (argv[2])
		{
			ft_putstr_fd("borsh: exit: too many arguments\n", STDERR_FILENO);
			*exit_status = 1;
			return (1);
		}
		status = (int)(ft_atoll(argv[1]) & 0xFF);
	}
	return (status);
}

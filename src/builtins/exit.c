#include "../../include/borsh.h"

static long	ft_atol(const char *str)
{
	int			index;
	long		result;
	int			sign;

	index = 0;
	result = 0L;
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

static bool	check_max_bounds(const char *str, int str_len)
{
	char	*max_str;
	int		max_len;

	if (str[0] == '-')
		max_str = "-9223372036854775808";
	else
		max_str = "9223372036854775807";
	max_len = ft_strlen(max_str);
	if (str_len > max_len)
		return (false);
	if (str_len < max_len)
		return (true);
	return (ft_strncmp(str, max_str, max_len) <= 0);
}

static bool	is_legal_numeric(const char *str)
{
	int		i;
	int		str_len;

	if (!str || !str[0])
		return (false);
	if (str[0] == '-' || str[0] == '+')
		i = 1;
	else
		i = 0;
	str_len = ft_strlen(str);
	if (!check_max_bounds(str, str_len))
		return (false);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
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
		if (!is_legal_numeric(argv[1]))
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
		status = (int)(ft_atol(argv[1]) & 0xFF);
	}
	return (status);
}

#include "../../include/borsh.h"

int	is_valid_var_name(char *var)
{
	int	i;

	if (!var || !var[0])
		return (0);
	if (!((var[0] >= 'A' && var[0] <= 'Z')
			|| (var[0] >= 'a' && var[0] <= 'z')
			|| var[0] == '_'))
		return (0);
	i = 1;
	while (var[i] && var[i] != '=')
	{
		if (!((var[i] >= 'A' && var[i] <= 'Z')
				|| (var[i] >= 'a' && var[i] <= 'z')
				|| (var[i] >= '0' && var[i] <= '9')
				|| var[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	print_end_exit(char *arg)
{
	ft_putstr_fd("export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
	return (1);
}

int	handle_export_arg(char *arg, char ***env)
{
	char	*equal_sign;
	int		var_len;
	char	*value;

	equal_sign = ft_strchr(arg, '=');
	if (!equal_sign)
	{
		if (!is_valid_var_name(arg))
			return (print_end_exit(arg));
		return (0);
	}
	var_len = equal_sign - arg;
	if (var_len == 0)
		return (print_end_exit(arg));
	if (!is_valid_var_name(arg))
		return (print_end_exit(arg));
	value = equal_sign + 1;
	if (set_env_var(env, arg, value) != 0)
	{
		ft_putstr_fd("export: failed to set variable\n", 2);
		return (1);
	}
	return (0);
}
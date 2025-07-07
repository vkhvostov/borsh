#include "../include/borsh.h"

int	increment_shlvl(char ***env)
{
	char	*shlvl_str;
	int		shlvl;
	char	*new_value;

	shlvl_str = get_env_var(*env, "SHLVL");
	if (!shlvl_str)
		shlvl = 0;
	else
		shlvl = ft_atoi(shlvl_str);
	shlvl++;
	new_value = ft_itoa(shlvl);
	if (!new_value)
		return (1);
	if (set_env_var(env, "SHLVL", new_value) != 0)
	{
		free(new_value);
		return (1);
	}
	free(new_value);
	return (0);
}

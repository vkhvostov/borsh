#include "../../include/borsh.h"

// removes the env variable at the given index
// from the env array
static void	remove_var(char ***env, int index)
{
	int	i;
	int	len;

	if (!env || !*env || index < 0)
		return ;
	len = 0;
	while ((*env)[len])
		len++;
	if (index >= len)
		return ;
	free((*env)[index]);
	i = index;
	while (i < len - 1)
	{
		(*env)[i] = (*env)[i + 1];
		i++;
	}
	(*env)[len - 1] = NULL;
}

// unsets a single environment variable by name
// 1 if the name is invalid, 0 otherwise
static int	unset_single_var(char *arg, char ***env)
{
	int		j;
	int		var_len;

	if (!is_valid_var_name(arg))
	{
		ft_putstr_fd("unset: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (1);
	}
	var_len = ft_strlen(arg);
	j = 0;
	while ((*env)[j])
	{
		if (ft_strncmp((*env)[j], arg, var_len) == 0 &&
			((*env)[j][var_len] == '=' || (*env)[j][var_len] == '\0'))
		{
			remove_var(env, j);
			break ;
		}
		j++;
	}
	return (0);
}

// unset builtin: removes env variables by name
int	builtin_unset(char **argv, char ***env)
{
	int	i;
	int	status;

	if (!argv[1])
		return (0);
	status = 0;
	i = 1;
	while (argv[i])
	{
		if (unset_single_var(argv[i], env))
			status = 1;
		i++;
	}
	return (status);
}

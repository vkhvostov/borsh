#include "../include/borsh.h"

static int	count_env_vars(char **env)
{
	int	count;

	count = 0;
	while (env[count])
		count++;
	return (count);
}

static char	**alloc_env_array(int size)
{
	char	**env;

	env = malloc(sizeof(char *) * (size + 1));
	if (!env)
	{
		set_last_exit_status(1);
		return (NULL);
	}
	env[size] = NULL;
	return (env);
}

static int	copy_env_vars(char **dst, char **src, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		dst[i] = ft_strdup(src[i]);
		if (!dst[i])
		{
			while (--i >= 0)
				free(dst[i]);
			free(dst);
			set_last_exit_status(1);
			return (0);
		}
		i++;
	}
	return (1);
}

char	**copy_environment(char **system_env)
{
	char	**env;
	int		env_count;

	if (!system_env)
		return (NULL);
	env_count = count_env_vars(system_env);
	env = alloc_env_array(env_count);
	if (!env)
		return (NULL);
	if (!copy_env_vars(env, system_env, env_count))
		return (NULL);
	return (env);
}

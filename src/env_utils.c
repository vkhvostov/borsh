#include "../include/borsh.h"

static int	count_env_vars(char **env)
{
	int	count;

	count = 0;
	while (env[count])
		count++;
	return (count);
}

static char	**alloc_env_array(int size, int *exit_status)
{
	char	**env;

	env = malloc(sizeof(char *) * (size + 1));
	if (!env)
	{
		*exit_status = 1;
		return (NULL);
	}
	env[size] = NULL;
	return (env);
}

static int	copy_env_vars(char **dst, char **src, int count, int *exit_status)
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
			*exit_status = 1;
			return (0);
		}
		i++;
	}
	return (1);
}

char	**copy_environment(char **system_env, int *exit_status)
{
	char	**env;
	int		env_count;

	if (!system_env)
		return (NULL);
	env_count = count_env_vars(system_env);
	env = alloc_env_array(env_count, exit_status);
	if (!env)
		return (NULL);
	if (!copy_env_vars(env, system_env, env_count, exit_status))
	{
		free(env);
		return (NULL);
	}
	return (env);
}

char	*get_env_var(char **env, const char *name)
{
	size_t	name_len;
	int		i;

	if (!env || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (env[i] + name_len + 1);
		i++;
	}
	return (NULL);
}

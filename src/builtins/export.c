#include "../../include/borsh.h"

// adds a new variable to the environment array
static int	env_add_var(char ***env, char *new_var)
{
	int		len;
	char	**new_env;
	int		i;
	char	*copy_var;

	copy_var = ft_strdup(new_var);
	if (!copy_var)
		return (1);
	len = 0;
	while ((*env)[len] != NULL)
		len++;
	new_env = malloc(sizeof(char *) * (len + 2));
	if (!new_env)
		return (free(copy_var), 1);
	i = 0;
	while (i < len)
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[len] = copy_var;
	new_env[len + 1] = NULL;
	free(*env);
	*env = new_env;
	return (0);
}

// returns the length of the variable name (up to '='/end)
static int	get_var_len(char *var_name)
{
	int	len;

	len = 0;
	while (var_name[len] && var_name[len] != '=')
		len++;
	return (len);
}

// creates a new environment entry string (NAME=VALUE)
static char	*create_env_entry(char *var_name, char *value, int var_len)
{
	int		value_len;
	char	*entry;
	int		j;

	value_len = ft_strlen(value);
	entry = malloc(var_len + 1 + value_len + 1);
	j = 0;
	if (!entry)
		return (NULL);
	while (j < var_len)
	{
		entry[j] = var_name[j];
		j++;
	}
	entry[var_len] = '=';
	j = 0;
	while (j < value_len)
	{
		entry[var_len + 1 + j] = value[j];
		j++;
	}
	entry[var_len + 1 + value_len] = '\0';
	return (entry);
}

// sets or updates an environment variable in the env array
int	set_env_var(char ***env, char *var_name, char *value)
{
	int		i;
	int		var_len;
	char	*new_entry;

	i = 0;
	var_len = get_var_len(var_name);
	new_entry = create_env_entry(var_name, value, var_len);
	if (!new_entry)
		return (1);
	while ((*env)[i])
	{
		if (ft_strncmp((*env)[i], var_name, var_len) == 0
			&& (*env)[i][var_len] == '=')
		{
			free((*env)[i]);
			(*env)[i] = new_entry;
			return (0);
		}
		i++;
	}
	if (env_add_var(env, new_entry) != 0)
		return (free(new_entry), 1);
	free(new_entry);
	return (0);
}

// sets environment variables or prints them if no arguments
int	builtin_export(char **argv, char ***env)
{
	int	i;
	int	status;

	if (!argv[1])
	{
		i = 0;
		while ((*env)[i])
		{
			printf("declare -x %s\n", (*env)[i]);
			i++;
		}
		return (0);
	}
	status = 0;
	i = 1;
	while (argv[i])
	{
		if (handle_export_arg(argv[i], env) != 0)
			status = 1;
		i++;
	}
	return (status);
}

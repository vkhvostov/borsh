#include "../../include/borsh.h"

int	env_add_var(char ***env, char *new_var)
{
	int		len;
	char	**new_env;
	int		i;
	char	*copy_var;

	copy_var = ft_strdup(new_var);
	if(!copy_var)
		return(1);
	len = 0;
	while ((*env)[len] != NULL)
		len++;
	new_env = malloc(sizeof(char *) * (len + 2)); // +1 for new var, +1 for NULL terminator
	if (!new_env)
	{
		free(copy_var);
		return (1);
	}
	i = 0;
	while(i < len)
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[len] = copy_var;
	new_env[len + 1] = NULL;
	*env = new_env;
	return (0);
}

int 	is_valid_var_name(char *var)
{
	int	i;

	i = 1;
	if (!var || (!((var[0] >= 'A' && var[0] <= 'Z') || (var[0] >= 'a' && var[0] <= 'z') || var[0] == '_')))
		return (0);
	while (var[i])
	{
		if (!((var[i] >= 'A' && var[i] <= 'Z') || (var[i] >= 'a' && var[i] <= 'z') ||
			(var[i] >= '0' && var[i] <= '9') || var[i] == '_'))
		return (0);
		i++;
	}
	return (1);
}

int	set_env_var(char ***env, char *var_name, char *value)
{
	int		i;
	int		var_len;
	int		value_len;
	char	*new_entry;
	int		j;

	i = 0;
	var_len = 0;
	while (var_name[var_len] && var_name[var_len] != '=')
		var_len++;

	// Compose new "VAR=value" string
	value_len = ft_strlen(value);
	
	new_entry = malloc(var_len + 1 + value_len + 1); // var + '=' + value + '\0'
	if (!new_entry)
		return 1;

	// Copy var_name part
	for (int j = 0; j < var_len; j++)
		new_entry[j] = var_name[j];

	new_entry[var_len] = '=';

	// Copy value part
	j = 0;
	while(j < value_len)
	{
		new_entry[var_len + 1 + j] = value[j];
		j++;
	}
	new_entry[var_len + 1 + value_len] = '\0';

	// Search for existing var
	while ((*env)[i])
	{
		if (ft_strncmp((*env)[i], var_name, var_len) == 0 && (*env)[i][var_len] == '=')
		{
			free((*env)[i]);
			(*env)[i] = new_entry;
			return 0;
		}
		i++;
	}

	// Not found — add new variable
	if (env_add_var(env, new_entry) != 0)
	{
		free(new_entry);
		return 1;
	}
	return 0;
}

int builtin_export(char **argv, char ***env)
{
	char	*arg;
	char	*equal_sign;
	int		i;
	int		var_len;
	char	var_name[100];
	char	*value;

	arg = argv[1];
	equal_sign = NULL;
	i = 0;
	if (!argv[1])
	{
		// No args, print environment variables
		int i = 0;
		while ((*env)[i])
		{
			printf("declare -x %s\n", (*env)[i]);
			i++;
		}
		return 0;
	}

	// Only handle one arg "VAR=value"
	while(arg[i])
	{
		if (arg[i] == '=')
		{
			equal_sign = &arg[i];
			break;
		}
		i++;
	}
	
	if (!equal_sign)
	{
		write(2, "export: invalid argument\n", 25);
		return 1;
	}

	// Split var and value manually
	var_len = equal_sign - arg;
	i = 0;
	while (i < var_len)
	{
		var_name[i] = arg[i];
		i++;
	}
	var_name[var_len] = '\0';
	value = equal_sign + 1;

	// You can add var_name validity check here if needed

	if (set_env_var(env, var_name, value) != 0)
	{
		write(2, "export: failed to set variable\n", 31);
		return 1;
	}
	return 0;
}

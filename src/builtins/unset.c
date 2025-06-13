#include "../../include/borsh.h"

static int is_valid_var_name(char *var)
{
	int i;

	if (!var || !var[0])
		return (0);
	// First character must be a letter or underscore
	if (!((var[0] >= 'A' && var[0] <= 'Z') || 
		  (var[0] >= 'a' && var[0] <= 'z') || 
		  var[0] == '_'))
		return (0);
	// Rest of the characters can be letters, numbers, or underscores
	i = 1;
	while (var[i])
	{
		if (!((var[i] >= 'A' && var[i] <= 'Z') || 
			  (var[i] >= 'a' && var[i] <= 'z') || 
			  (var[i] >= '0' && var[i] <= '9') || 
			  var[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static void remove_var(char ***env, int index)
{
	int i;
	int len;

	if (!env || !*env || index < 0)
		return;

	len = 0;
	while ((*env)[len])
		len++;

	if (index >= len)
		return;

	// Free the variable to be removed
	free((*env)[index]);

	// Shift remaining variables
	i = index;
	while (i < len - 1)
	{
		(*env)[i] = (*env)[i + 1];
		i++;
	}
	(*env)[len - 1] = NULL;
}

int builtin_unset(char **argv, char ***env)
{
	int i;
	int j;
	int var_len;
	int status;

	if (!argv[1])
		return (0);

	status = 0;
	i = 1;
	while (argv[i])
	{
		if (!is_valid_var_name(argv[i]))
		{
			ft_putstr_fd("unset: `", 2);
			ft_putstr_fd(argv[i], 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			status = 1;
			i++;
			continue;
		}

		var_len = ft_strlen(argv[i]);
		j = 0;
		while ((*env)[j])
		{
			if (ft_strncmp((*env)[j], argv[i], var_len) == 0 && 
				((*env)[j][var_len] == '=' || (*env)[j][var_len] == '\0'))
			{
				remove_var(env, j);
				break;
			}
			j++;
		}
		i++;
	}
	return (status);
} 
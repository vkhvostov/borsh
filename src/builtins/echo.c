#include "../../include/borsh.h"

static int	is_dash_n(char *str)
{
	int	i;

	i = 1;
	if (!str || str[0] != '-')
		return (0);
	while (str[i] == 'n')
		i++;
	return (str[i] == '\0');
}

// implements the echo, prints its arguments to stdout
int	builtin_echo(char **argv)
{
	int	i;
	int	new_line;

	i = 1;
	new_line = 1;
	while (argv[i] && is_dash_n(argv[i]))
	{
		new_line = 0;
		i++;
	}
	while (argv[i])
	{
		if (argv[i][0] != '\0')
		{
			printf("%s", argv[i]);
			if (argv[i + 1])
				printf(" ");
			i++;
		}
	}
	if (new_line)
		printf("\n");
	return (0);
}

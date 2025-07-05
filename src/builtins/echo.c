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

static void	print_argument_if_not_empty(char *arg, int *first)
{
	if (arg[0] != '\0')
	{
		if (!*first)
			printf(" ");
		printf("%s", arg);
		*first = 0;
	}
}

// implements the echo, prints its arguments to stdout
int	builtin_echo(char **argv)
{
	int	i;
	int	new_line;
	int	first;

	i = 1;
	first = 1;
	new_line = 1;
	while (argv[i] && is_dash_n(argv[i]))
	{
		new_line = 0;
		i++;
	}
	while (argv[i])
	{
		if (argv[i][0] != '\0')
			print_argument_if_not_empty(argv[i], &first);
		i++;
	}
	if (new_line)
		printf("\n");
	return (0);
}

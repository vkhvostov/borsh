#include "../../include/borsh.h"

// implements the echo, prints its arguments to stdout
int	builtin_echo(char **argv)
{
	int	i;
	int	new_line;

	i = 1;
	new_line = 1;
	if (argv[1] && ft_strcmp(argv[1], "-n") == 0)
	{
		new_line = 0;
		i++;
	}
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
	if (new_line)
		printf("\n");
	return (0);
}

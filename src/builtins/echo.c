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

static void	print_args(char **argv, int start_idx, int *first, int *was_empty)
{
	if (!*first && (*was_empty || argv[start_idx][0] != '\0'
		|| argv[start_idx - 1][0] != '\0'))
		printf(" ");
	printf("%s", argv[start_idx]);
	*was_empty = (argv[start_idx][0] == '\0');
	*first = 0;
}

int	builtin_echo(char **argv)
{
	int	i;
	int	new_line;
	int	first;
	int	was_empty;

	i = 1;
	first = 1;
	was_empty = 0;
	new_line = 1;
	while (argv[i] && is_dash_n(argv[i]))
	{
		new_line = 0;
		i++;
	}
	while (argv[i])
	{
		print_args(argv, i, &first, &was_empty);
		i++;
	}
	if (new_line)
		printf("\n");
	return (0);
}

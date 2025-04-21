#include "../include/borsh.h"

int read_input(void)
{
	char *line;

	while (1)
	{
		line = readline("borsh> ");
		if (!line)
			break;
		if (*line)
			add_history(line);
		printf("You typed: %s\n", line);
		free(line);
	}
	return 0;
}

int main(int count, char **args)
{
	if (count == 0 && args)
		return (1);
	read_input();
	return (0);
}

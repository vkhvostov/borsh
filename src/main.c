#include "../include/borsh.h"

int main(int count, char **args)
{
	if (count == 0 && args)
		return (1);
	return (0);
}

// Code to test the get_next_line function
/* #include <fcntl.h>
#include <stdio.h>
int main(int count, char *args[]) {
		if (count != 1) return args[1] == NULL;
		int file = STDIN_FILENO;
		char *line = get_next_line(file);
		while (line) {
			printf("%s", line);
			line = get_next_line(file);
		}
		return 0;
} */

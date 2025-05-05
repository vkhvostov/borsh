#include "../include/borsh.h"

char *empty_string(void)
{
	char	*str;

	str = malloc(1);
	if (str == NULL)
		return NULL;
	str[0] = '\0';
	return str;
}

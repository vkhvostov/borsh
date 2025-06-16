#include "../../include/borsh.h"

static int	g_exit_status = 0;

int	get_last_exit_status(void)
{
	return (g_exit_status);
}

void	set_last_exit_status(int status)
{
	g_exit_status = status;
}

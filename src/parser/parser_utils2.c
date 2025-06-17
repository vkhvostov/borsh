#include "../../include/borsh.h"

void	memory_error_handler(void)
{
	set_last_exit_status(1);
	return ;
}

#include "../../include/borsh.h"

void	memory_error_handler(void)
{
	set_last_exit_status(1);
	return ;
}

void	handle_redirs(t_token **tokens, t_command	*current)
{
	handle_redir_tokens(&current->redirs, tokens, (*tokens)->type);
}

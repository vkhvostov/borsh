#include "../../include/borsh.h"

void	handle_sigint_interactive(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigint_noninteractive(int sig)
{
	(void)sig;
}

void	set_interactive_mode(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_interactive;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	set_non_interactive_mode(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_noninteractive;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_DFL);
}

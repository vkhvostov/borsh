#include "../../include/borsh.h"

void	handle_sigint_interactive(int sig)
{
	(void)sig;
	g_signal_status = 130;
	rl_replace_line("", 0);
	rl_on_new_line();
	ft_putstr_fd("\n", STDERR_FILENO);
	rl_redisplay();
}

void	handle_sigint_noninteractive(int sig)
{
	(void)sig;
	ft_putstr_fd("\n", STDERR_FILENO);
}

void	set_interactive_mode(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_interactive;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
	update_terminal_settings(false);  // Disable VQUIT for interactive shell mode
}

void	set_non_interactive_mode(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint_noninteractive;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);  /* Keep SIGQUIT ignored in the parent shell */
	update_terminal_settings(true);  // Enable VQUIT for command execution
}

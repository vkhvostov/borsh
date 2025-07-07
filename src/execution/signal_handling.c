#include "../../include/borsh.h"

int	g_signal_status = 0;

static void	handle_sigint(int sig)
{
	(void)sig;
	g_signal_status = 130;
	write(STDERR_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	handle_sigquit(int sig)
{
	g_signal_status = sig;
}

void	setup_signal_handlers(int *exit_status)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = SA_RESTART;
	sigemptyset(&sa_int.sa_mask);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		ft_putstr_fd("borsh: sigaction for SIGINT failed\n", 2);
		*exit_status = 1;
		return ;
	}
	sa_quit.sa_handler = handle_sigquit;
	sa_quit.sa_flags = SA_RESTART;
	sigemptyset(&sa_quit.sa_mask);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		ft_putstr_fd("borsh: sigaction for SIGQUIT failed\n", 2);
		*exit_status = 1;
		return ;
	}
}

void	reset_signal_handlers(int *exit_status)
{
	if (signal(SIGINT, SIG_DFL) == SIG_ERR)
	{
		ft_putstr_fd("borsh: signal reset for SIGINT failed\n", 2);
		*exit_status = 1;
		return ;
	}
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
	{
		ft_putstr_fd("borsh: signal reset for SIGQUIT failed\n", 2);
		*exit_status = 1;
		return ;
	}
}

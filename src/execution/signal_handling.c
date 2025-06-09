#include "../../include/borsh.h"

// Global variable to store only the signal number
volatile sig_atomic_t g_signal_status = 0;

// Private variable for exit status (not accessible outside this file)
static int last_exit_status = 0;

static void handle_sigint(int sig)
{
	g_signal_status = sig;
	write(STDERR_FILENO, "\n", 1);  // Safe to use in signal handler
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	set_last_exit_status(130);  // 128 + SIGINT(2)
}

static void handle_sigquit(int sig)
{
	g_signal_status = sig;
	// In interactive mode, we ignore SIGQUIT
	// In child processes, the default handler will create a core dump
}

void setup_signal_handlers(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = SA_RESTART;
	sigemptyset(&sa_int.sa_mask);
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("sigaction for SIGINT failed");
		set_last_exit_status(1);  // Signal error
		return;
	}

	sa_quit.sa_handler = handle_sigquit;
	sa_quit.sa_flags = SA_RESTART;
	sigemptyset(&sa_quit.sa_mask);
	if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
	{
		perror("sigaction for SIGQUIT failed");
		set_last_exit_status(1);  // Signal error
		return;
	}
}

void reset_signal_handlers(void)
{
	if (signal(SIGINT, SIG_DFL) == SIG_ERR)
	{
		perror("signal reset for SIGINT failed");
		set_last_exit_status(1);  // Signal error
		return;
	}
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
	{
		perror("signal reset for SIGQUIT failed");
		set_last_exit_status(1);  // Signal error
		return;
	}
}

int get_last_exit_status(void)
{
	return last_exit_status;
}

void set_last_exit_status(int status)
{
	last_exit_status = status;
} 
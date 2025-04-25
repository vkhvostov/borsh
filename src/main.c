#include "../include/borsh.h"

static void	hide_ctrl_c_echo(void)
{
	struct termios	term;
	// get current terminal settings
	tcgetattr(STDIN_FILENO, &term);
	// disable echoing of control chars
	term.c_lflag &= ~ECHOCTL;
	// apply the settings
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// handling CTRL+C
static void	handle_sigint(int sig)
{
	(void)sig;
	printf("\n");
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

int	read_input(void)
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

int	main(int count, char **args)
{
	if (count == 0 && args)
		return (1);
	hide_ctrl_c_echo();
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	read_input();
	return (0);
}

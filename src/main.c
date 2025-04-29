#include "../include/borsh.h"

// for debugging, will delete in the future
void	print_tokens(t_token *token_list)
{
	t_token	*temp = token_list;

	while(temp)
	{
		printf("Token: Type: %d, Value: %s\n", temp->type, temp->value);
		temp = temp->next;
	}
}

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

char	*read_input(void)
{
	char *line;

	line = readline("borsh> ");
	// If user presses Ctrl+D
	if (!line)
		return (NULL);
	// If the line is not empty
	if (*line)
		add_history(line);
	return (line);
}

int	main()
{
	char	*input;
	t_token	*token_list;

	hide_ctrl_c_echo();
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		input = read_input();
		if (!input)
		{
			printf("exit\n");
			break;
		}
		if (input[0] == '\0')
		{
			free(input);
			continue;
		}
		token_list = lexer(input);
		print_tokens(token_list);
		free(input);
	}
	return (0);
}

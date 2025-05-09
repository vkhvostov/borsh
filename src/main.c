#include "../include/borsh.h"

int g_exit_status = 0;

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

// for debugging, will delete in the future
void	print_redirects(t_redirect *redir_list, const char *label)
{
	while (redir_list)
	{
		printf("  %s: (%d) %s\n", label, redir_list->type, redir_list->file);
		redir_list = redir_list->next;
	}
}

// for debugging, will delete in the future
void	print_commands(t_command *cmd_list)
{
	int	i;

	while (cmd_list)
	{
		printf("Command:\n");
		if (cmd_list->cmd_name)
			printf("  cmd_name: %s\n", cmd_list->cmd_name);
		if (cmd_list->argv)
		{
			i = 0;
			while (cmd_list->argv[i])
			{
				printf("  argv[%d]: %s\n", i, cmd_list->argv[i]);
				i++;
			}
		}
		print_redirects(cmd_list->in_redir, "in_redir");
		print_redirects(cmd_list->out_redir, "out_redir");
		printf("----\n");
		cmd_list = cmd_list->next;
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
	char		*input;
	t_token		*token_list;
	t_command	*cmd_list;

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
		token_list = lexer(expand_variables(input));
		if (!token_list)
		{
			free(input);
			continue;
		}
		cmd_list = parse_tokens(token_list);
		if (!cmd_list)
		{
			free_tokens(token_list);
			free(input);
			continue;
		}
		print_commands(cmd_list);
		// print_tokens(token_list);
		free_tokens(token_list);
		free_commands(cmd_list);
		free(input);
	}
	return (0);
}

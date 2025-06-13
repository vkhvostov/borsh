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

char	*read_input(void)
{
	char	*line;
	char	cwd[1024];
	char	*prompt;
	size_t	prompt_len;
	char	*prefix;

	prefix = "borsh> ";
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		// +1 for space, +1 for '\0'
		prompt_len = ft_strlen(prefix) + ft_strlen(cwd) + 2;
		prompt = malloc(prompt_len);
		if(!prompt)
			return (NULL);
		ft_strlcpy(prompt, prefix, prompt_len);
		ft_strlcat(prompt, cwd, prompt_len);
		ft_strlcat(prompt, " ", prompt_len);
	}
	else
	{
		prompt = ft_strdup("borsh> ");
		if (!prompt)
			return (NULL);
	}

	line = readline(prompt);
	free(prompt);
	// If user presses Ctrl+D
	if (!line)
		return (NULL);
	// If the line is not empty
	if (*line)
		add_history(line);
	return (line);
}

int	main(int argc, char **argv, char **env)
{
	char	*input;
	char	*expanded_input;
	t_token	*token_list;
	t_command	*cmd_list;

	(void)argc;
	(void)argv;
	hide_ctrl_c_echo();
	setup_signal_handlers();
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
		expanded_input = expand_variables(input, env);
		if (!expanded_input)
		{
			set_last_exit_status(1);
			free(input);
			continue;
		}
		token_list = lexer(expanded_input);
		free(expanded_input);
		if (!token_list)
		{
			free(input);
			continue;
		}

		// print_tokens(token_list);

		cmd_list = parse_tokens(token_list, env);
		if (!cmd_list)
		{
			free_tokens(token_list);
			free(input);
			continue;
		}
		
		// print_commands(cmd_list);

		execute(cmd_list, &env);

		free_tokens(token_list);
		free_commands(cmd_list);
		free(input);
	}
	return (get_last_exit_status());
}

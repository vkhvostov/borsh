#include "../include/borsh.h"

static void	hide_ctrl_c_echo(void)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
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
		prompt_len = ft_strlen(prefix) + ft_strlen(cwd) + 2;
		prompt = malloc(prompt_len);
		if (!prompt)
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
	if (!line)
		return (NULL);
	if (*line)
		add_history(line);
	return (line);
}

static char **copy_environment(char **system_env)
{
	char **env = NULL;
	int env_count = 0;
	int i;

	if (!system_env)
		return NULL;

	// Count environment variables
	while (system_env[env_count])
		env_count++;

	// Allocate space for environment array
	env = malloc(sizeof(char *) * (env_count + 1));
	if (!env)
		return NULL;

	// Copy each environment variable
	for (i = 0; i < env_count; i++)
	{
		env[i] = ft_strdup(system_env[i]);
		if (!env[i])
		{
			// If allocation fails, free everything allocated so far
			while (--i >= 0)
				free(env[i]);
			free(env);
			return NULL;
		}
	}
	env[env_count] = NULL;

	return env;
}

int	main(int argc, char **argv, char **env)
{
	char	*input;
	char	*expanded_input;
	t_token	*token_list;
	t_command	*cmd_list;
	char	**shell_env;

	(void)argc;
	(void)argv;
	hide_ctrl_c_echo();
	setup_signal_handlers();

	// Make a copy of the environment
	shell_env = copy_environment(env);
	if (!shell_env)
		return (1);

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
		expanded_input = expand_variables(input, shell_env);
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

		cmd_list = parse_tokens(token_list, shell_env);
		if (!cmd_list)
		{
			free_tokens(token_list);
			free(input);
			continue;
		}
		
		execute(cmd_list, &shell_env);

		free_tokens(token_list);
		free_commands(cmd_list);
		free(input);
	}

	// Free the environment array
	if (shell_env)
	{
		int i = 0;
		while (shell_env[i])
		{
			free(shell_env[i]);
			i++;
		}
		free(shell_env);
	}

	return (get_last_exit_status());
}

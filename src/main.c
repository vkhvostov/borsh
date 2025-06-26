#include "../include/borsh.h"

// creates the shell prompt string 
// using the current working directory
static char	*create_prompt(char *cwd)
{
	char	*prompt;
	size_t	prompt_len;
	char	*prefix;

	prefix = "borsh> ";
	prompt_len = ft_strlen(prefix) + ft_strlen(cwd) + 2;
	prompt = malloc(prompt_len);
	if (!prompt)
		return (NULL);
	ft_strlcpy(prompt, prefix, prompt_len);
	ft_strlcat(prompt, cwd, prompt_len);
	ft_strlcat(prompt, " ", prompt_len);
	return (prompt);
}

// Reads a line of input from the user
// displays the prompt. Adds non-empty lines to history.
static char	*read_input(void)
{
	char	*line;
	char	cwd[1024];
	char	*prompt;

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		prompt = create_prompt(cwd);
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

// initializes the shell environment and signal handlers
static int	init_shell(char ***shell_env, char **env, int *exit_status)
{
	hide_ctrl_c_echo();
	setup_signal_handlers(exit_status);
	*shell_env = copy_environment(env, exit_status);
	if (!*shell_env)
		return (1);
	return (0);
}

// Processes a single line of user input: expands variables,
// tokenizes, parses, and executes.
static int	process_input(char *input, char ***shell_env, int *exit_status)
{
	char		*expanded_input;
	t_token		*token_list;
	t_command	*cmd_list;

	expanded_input = expand_variables(input, *shell_env, exit_status);
	if (!expanded_input)
	{
		*exit_status = 1;
		return (0);
	}
	token_list = lexer(expanded_input, exit_status);
	free(expanded_input);
	if (!token_list)
		return (0);
	cmd_list = parse_tokens(token_list, *shell_env, exit_status);
	if (!cmd_list)
	{
		free_tokens(token_list);
		return (0);
	}
	execute(cmd_list, shell_env, exit_status);
	free_tokens(token_list);
	free_commands(cmd_list);
	return (1);
}

// initializes the shell, runs the main loop, and cleans up
int	main(int argc, char **argv, char **env)
{
	char	*input;
	char	**shell_env;
	int		exit_status;

	(void)argc;
	(void)argv;
	exit_status = 0;
	if (init_shell(&shell_env, env, &exit_status))
		return (1);
	while (1)
	{
		input = read_input();
		if (!input)
			break ;
		if (input[0] != '\0')
			process_input(input, &shell_env, &exit_status);
		free(input);
	}
	printf("exit\n");
	free_shell_env(shell_env);
	return (exit_status);
}

#include "../include/borsh.h"

// creates the shell prompt string with blue background color
// and the current working directory
static char	*create_prompt(char *cwd)
{
	char	*prompt;
	size_t	prompt_len;
	char	*prefix;
	char	*color_start;
	char	*color_end;

	prefix = "borsh> ";
	color_start = "\033[44m";
	color_end = "\033[0m";
	prompt_len = ft_strlen(color_start) + ft_strlen(cwd)
		+ ft_strlen(color_end) + ft_strlen(prefix) + 2;
	prompt = malloc(prompt_len);
	if (!prompt)
		return (NULL);
	prompt[0] = '\0';
	ft_strlcpy(prompt, color_start, prompt_len);
	ft_strlcat(prompt, cwd, prompt_len);
	ft_strlcat(prompt, color_end, prompt_len);
	ft_strlcat(prompt, "\n", prompt_len);
	ft_strlcat(prompt, prefix, prompt_len);
	return (prompt);
}

// Reads a line of input from the user
// displays the prompt. Adds non-empty lines to history.
static char	*read_input(void)
{
	char	*line;
	char	*cwd;
	char	*prompt;

	cwd = getcwd(NULL, 0);
	if (cwd)
		prompt = create_prompt(cwd);
	else
	{
		prompt = ft_strdup("borsh> ");
		if (!prompt)
			return (NULL);
	}
	line = readline(prompt);
	free(prompt);
	free(cwd);
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
	if (increment_shlvl(shell_env) != 0)
	{
		free_shell_env(*shell_env);
		*exit_status = 1;
		return (1);
	}
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
	free_tokens(token_list);
	set_non_interactive_mode();
	execute(cmd_list, shell_env, exit_status);
	set_interactive_mode();
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
	// printf("exit\n");
	free_shell_env(shell_env);
	return (exit_status);
}

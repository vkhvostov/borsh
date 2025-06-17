#include "../include/borsh.h"

char	*create_prompt(char *cwd)
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

char	*read_input(void)
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

static int	init_shell(char ***shell_env, char **env)
{
	hide_ctrl_c_echo();
	setup_signal_handlers();
	*shell_env = copy_environment(env);
	if (!*shell_env)
		return (1);
	return (0);
}

static int	process_input(char *input, char ***shell_env)
{
	char		*expanded_input;
	t_token		*token_list;
	t_command	*cmd_list;

	expanded_input = expand_variables(input, *shell_env);
	if (!expanded_input)
	{
		set_last_exit_status(1);
		return (0);
	}
	token_list = lexer(expanded_input);
	free(expanded_input);
	if (!token_list)
		return (0);
	cmd_list = parse_tokens(token_list, *shell_env);
	if (!cmd_list)
	{
		free_tokens(token_list);
		return (0);
	}
	execute(cmd_list, shell_env);
	free_tokens(token_list);
	free_commands(cmd_list);
	return (1);
}

int	main(int argc, char **argv, char **env)
{
	char	*input;
	char	**shell_env;

	(void)argc;
	(void)argv;
	if (init_shell(&shell_env, env))
		return (1);
	while (1)
	{
		input = read_input();
		if (!input)
			break ;
		if (input[0] != '\0')
			process_input(input, &shell_env);
		free(input);
	}
	printf("exit\n");
	free_shell_env(shell_env);
	return (get_last_exit_status());
}

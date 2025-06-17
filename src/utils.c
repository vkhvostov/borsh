#include "../include/borsh.h"

int	is_word_char(char c)
{
	return (c && c != '|' && c != '<' && c != '>'
			&& c != '"' && c != '\'' && c != ' ');
}

void	skip_whitespace(char *input, int *i)
{
	while(input[*i] && (input[*i] == 32 || (input[*i] >= 9 && input[*i] <= 13)))
		(*i)++;
}

char *empty_string(void)
{
	char	*str;

	str = malloc(1);
	if (str == NULL)
		return NULL;
	str[0] = '\0';
	return str;
}

void	free_shell_env(char **shell_env)
{
	int i;

	i = 0;
	while (shell_env[i])
	{
		free(shell_env[i]);
		i++;
	}
	free(shell_env);
}

void	hide_ctrl_c_echo(void)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}


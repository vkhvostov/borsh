#include "../include/borsh.h"

int	is_word_char(char c)
{
	return (c && c != '|' && c != '<' && c != '>'
		&& c != '"' && c != '\'' && c != ' ' && c != '\\');
}

void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && (input[*i] == 32
			|| (input[*i] >= 9 && input[*i] <= 13)))
		(*i)++;
}

char	*empty_string(void)
{
	char	*str;

	str = malloc(1);
	if (str == NULL)
		return (NULL);
	str[0] = '\0';
	return (str);
}

void	free_shell_env(char **shell_env)
{
	int	i;

	i = 0;
	while (shell_env[i])
	{
		free(shell_env[i]);
		i++;
	}
	free(shell_env);
}

void	update_terminal_settings(bool enable_vquit)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) < 0)
		return;
	if (enable_vquit)
	{
		term.c_cc[VQUIT] = 0x1C;
		term.c_lflag |= (ISIG | ECHOCTL);
	}
	else
	{
		term.c_cc[VQUIT] = _POSIX_VDISABLE;
		term.c_lflag |= ECHOCTL;
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

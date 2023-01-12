#include "minishell.h"
#include <readline/readline.h>

int main()
{
	char	*ch;
	char	**line;

	while (1)
	{
		//SHLVL++;
		ch = readline("minishell-$ ");
		line = ft_split(ch, ' ');
		if (!ft_strncmp(line[0], "cd", 2) && !line[0][2])
			cd(line[1]);
	}
	return (0);
}

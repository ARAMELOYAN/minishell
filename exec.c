/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 16:16:45 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 14:14:12 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec(t_cmd *cmd, char **envp)
{
	t_var	var;

	if (fork() == 0)
	{
		if (!access(cmd->arg[0], X_OK))
			execve(cmd->arg[0], cmd->arg, envp);
		var.path_split = ft_split(getenv("PATH"), ':');
		var.iter_i = 0;
		while (var.path_split[var.iter_i])
			if (find_and_run(cmd, &var, envp))
				return (errno);
		exit (127);
	}
	else
		wait(&g_status);
	g_status %= 255;
	if (g_status == 127)
		return (0);
	return (1);
}

void	pwd(void)
{
	t_var	var;

	printf("%s\n", getcwd(var.buffer, 200));
}

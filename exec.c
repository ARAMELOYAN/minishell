/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 16:16:45 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/04 12:45:21 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


int exec(cmd_t *cmd, char **envp)
{
	var_t	var;

	if (fork() == 0)
	{
		if (!access(cmd->arg[0], X_OK))
			execve(cmd->arg[0], cmd->arg, envp);
		var.path_split = ft_split(getenv("PATH"), ':');
		var.iter_i = 0;
		while (var.path_split[var.iter_i])
		{
			var.cmd_path = ft_strjoin(var.path_split[var.iter_i], "/");
			var.cmd_path_1 = ft_strjoin(var.cmd_path, cmd->arg[0]);
			free(var.cmd_path);
			if (!access(var.cmd_path_1, X_OK))
			{
				if (execve(var.cmd_path_1, cmd->arg, envp) == -1)
				{
					perror("msh: execve");
					return (errno);
				}
				free(var.cmd_path_1);
			}
			var.iter_i++;
		}
		exit (127);
	}
	else
		wait(&g_status);
	g_status %= 255;
	if (g_status == 127)
		return (0);
	return (1);
}

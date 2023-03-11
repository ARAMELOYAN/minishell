/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 16:16:45 by aeloyan           #+#    #+#             */
/*   Updated: 2023/03/10 15:20:29 by tumolabs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int exec(cmd_t *cmd, char **envp)
{
	var_t	var;

	//command not foundo
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
					perror(0);
					return (errno);
				}
				free(var.cmd_path_1);
			}
			var.iter_i++;
		}
		exit(0);
	}
	else
		wait(0);
	return (1);
}

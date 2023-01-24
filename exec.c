/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 16:16:45 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/22 16:49:31 by tumolabs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int exec(cmd_t *cmd)
{
	var_t	var;

	//command not foundo
	if (fork() == 0)
	{
		if (!access(cmd->arg[0], X_OK))
			execve(cmd->arg[0], cmd->arg, NULL);
		var.path_split = ft_split(getenv("PATH"), ':');
		var.iter_i = 0;
		while (var.path_split[var.iter_i])
		{
			var.directory = opendir(var.path_split[var.iter_i]);
			if (!var.directory)
			{
				perror(0);
				return (errno);
			}
			entity = readdir(var.directory);
			if (!entity)
			{
				perror(0);
				return (errno);
			}
			while (entity)
			{
				if (!ft_strncmp(entity->d_name, cmd->arg[0], ft_strlen(cmd->arg[0]))
						&& !entity->d_name[ft_strlen(cmd->arg[0])])
				{
					var.cmd_path = ft_strjoin(var.path_split[var.iter_i], "/");
					var.cmd_path_1 = ft_strjoin(var.cmd_path, entity->d_name);
					free(var.cmd_path);
					if (execve(var.cmd_path_1, cmd->arg, NULL) == -1)
					{
						perror(0);
						return (errno);
					}
					free(var.cmd_path_1);
				}
				entity = readdir(var.directory);
			}
			closedir(var.directory);
			var.iter_i++;
		}
		exit(0);
	}
	else
		wait(0);
	return (1);
}

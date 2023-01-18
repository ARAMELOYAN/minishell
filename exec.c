/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 16:16:45 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/18 19:19:20 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int exec(cmd_t *cmd)
{
	//command not foundo
	if (fork() == 0)
	{
		execve(cmd->arg[0], cmd->arg, NULL);
		path_split = ft_split(getenv("PATH"), ':');
		iter_i = 0;
		while (path_split[iter_i])
		{
			directory = opendir(path_split[iter_i]);
			if (!directory)
			{
				perror(0);
				return (errno);
			}
			entity = readdir(directory);
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
					cmd_path = ft_strjoin(path_split[iter_i], "/");
					cmd_path_1 = ft_strjoin(cmd_path, entity->d_name);
					free(cmd_path);
					if (execve(cmd_path_1, cmd->arg, NULL) == -1)
					{
						perror(0);
						return (errno);
					}
					free(cmd_path_1);
				}
				entity = readdir(directory);
			}
			closedir(directory);
			iter_i++;
		}
	}
	else
		wait(0);
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 16:16:45 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/11 14:50:27 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int exec(char *str)
{//absolute pati hamar petq e grel kod@ aystex
	//command not found
	ex_inp_spl = ft_split(str, ' ');
	path_split = ft_split(getenv("PATH"), ':');
	iter_i = 0;
	while (path_split[iter_i])
	{
		directory = opendir(path_split[iter_i]);
		if (!directory)
		{
			printf("1%s\n", strerror(errno));
			return (errno);
		}
		entity = readdir(directory);
		if (!entity)
		{
			printf("2%s\n", strerror(errno));
			return (errno);
		}
		while (entity)
		{
			if (!ft_strncmp(entity->d_name, ex_inp_spl[0], ft_strlen(ex_inp_spl[0]))
					&& !entity->d_name[ft_strlen(ex_inp_spl[0])])
			{
				cmd_path = ft_strjoin(path_split[iter_i], "/");
				cmd_path_1 = ft_strjoin(cmd_path, entity->d_name);
				free(cmd_path);
				if (execve(cmd_path_1, ex_inp_spl, NULL) == -1)
				{
					printf("3%s\n%s\n%s\n", strerror(errno), cmd_path, ex_inp_spl[0]);
					return (errno);
				}
				printf("\nHELLO");
				free(cmd_path_1);
			}
			entity = readdir(directory);
		}
		closedir(directory);
		iter_i++;
	}
	return (0);
}

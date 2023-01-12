/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:58:47 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/11 17:19:18 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	pwd()
{
	printf("%s\n", getcwd(buffer, 200));
}

int cd(char *str)
{
	pwd();
	if (!str || !*str)
	{
		if (chdir("/USERS") == -1)
			printf("%s\n", strerror(errno));
		if (chdir(getenv("USER")) == -1)
			printf("%s\n", strerror(errno));
	}
	else
		if (chdir(str) == -1)
			printf("%s\n", strerror(errno));
	pwd();
	return (0);
}

void echos(char *str, int arg)
{
	if (arg == 1)
		printf("%s", str);
	else
		printf("%s\n", str);
}

int	main(int ac, char **av)
{
	cd(av[1]);
	return (0);
}

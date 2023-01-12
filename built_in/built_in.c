/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:58:47 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/12 16:17:51 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	export(char *str)
{
	char	**envp;

	envp = environ;
	while (*envp)
	{
		if (!ft_strncmp(*envp, str, ft_strchr(str, '=') - str + 1))
		{
			*envp = ft_strdup(str);//jshtel, ardyoq peat e malloq unena aystegh
			break ;
		}
		envp++;
	}
	if (!*envp)
	{
		*envp = ft_strdup(str);//jshtel, ardyoq peat e malloq unena aystegh
		*(++envp) = NULL;
	}
}

void	pwd()
{
	printf("%s\n", getcwd(buffer, 200));
}

int cd(char *str)
{
	pwd();
	if (!str || !*str)
	{
		if (chdir(getenv("HOME")) == -1)
			perror(strerror(errno));
		else
			export("PWD=");
	}
	else
	{
		if (chdir(str) == -1)
			perror(strerror(errno));
		else
			export("PWD=");
	}
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
	export(av[1]);
	return (0);
}

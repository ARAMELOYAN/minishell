/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:58:47 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/19 16:57:50 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset(cmd_t *cmd)
{
	char	**envp;

	while (*(++cmd->arg))
	{
		envp = environ;
		while (*envp)
		{
			if (!ft_strncmp(*envp, *cmd->arg, ft_strchr(*envp, '=') - *envp))
			{
				envp--;
				while (*(++envp))
					*envp = envp[1];
				break ;
			}
			envp++;
		}
	}
}

void	export(cmd_t *cmd)
{
	char	**envp;

	while (*(++cmd->arg))
	{
	envp = environ;
		while (*envp)
		{
			if (!ft_strncmp(*envp, *cmd->arg, ft_strchr(*cmd->arg, '=') - *cmd->arg + 1))
			{
				*envp = ft_strdup(*cmd->arg);//jshtel, ardyoq petq e malloq unena aystegh
				break ;
			}
			envp++;
		}
		if (!*envp)
		{
			*envp = ft_strdup(*cmd->arg);//jshtel, ardyoq petq e malloq unena aystegh
			*(++envp) = NULL;
		}
	}
}

void	pwd()
{
	printf("%s\n", getcwd(buffer, 200));
}

void	maneur(void)
{
	cmd_t	*cmd_1;

	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->arg = (char **)malloc(sizeof(char *) * 4);
	cmd_1->arg[0] = ft_strdup("export");
	cmd_1->arg[1] = ft_strjoin("OLDPWD=", getenv("PWD"));
	cmd_1->arg[2] = ft_strjoin("PWD=", getcwd(buffer, 200));
	cmd_1->arg[3] = NULL;
	cmd_1->next = NULL;
	export(cmd_1);
	del_cmd(cmd_1);
}

int cd(cmd_t *cmd)
{
	if (!*(++cmd->arg))
	{
		if (!getenv("HOME") || chdir(getenv("HOME")) == -1)
		{
			perror(0);
		}
		else
			maneur();
	}
	else
	{
		if (chdir(*(cmd->arg)) == -1)
			perror(strerror(errno));
		else
			maneur();
	}
	return (0);
}

void echom(cmd_t *cmd)
{
	iter_i = 0;
	if (cmd->arg[1] && !ft_strncmp(cmd->arg[1], "-", 1))
		while (cmd->arg[1][++iter_i])
			if(ft_strncmp(cmd->arg[1] + iter_i, "n", 1))
				break;
	if (cmd->arg[1] && !cmd->arg[1][iter_i])
	{
		iter_i = 2;
		while (cmd->arg[iter_i] && printf("%s", cmd->arg[iter_i]))
		{
			if (cmd->arg[++iter_i])
				printf(" ");
		}
	}
	else
	{
		iter_i = 1;
		while (cmd->arg[iter_i] && printf("%s", cmd->arg[iter_i]))
		{
			if (cmd->arg[++iter_i])
				printf(" ");
		}
		printf("\n");
	}
}

void	env()
{
	iter_i = 0;
	while (environ[iter_i])
		printf("%s\n", environ[iter_i++]);
}

void exitm(cmd_t *cmd)
{
	printf("exit\n");
	if (cmd->arg[1])
		exit(ft_atoi(cmd->arg[1]));
	exit(0);
}

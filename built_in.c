/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:58:47 by aeloyan           #+#    #+#             */
/*   Updated: 2023/02/08 16:20:38 by tumolabs         ###   ########.fr       */
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
	var_t	var;

	printf("%s\n", getcwd(var.buffer, 200));
}

void	manevr(void)
{
	cmd_t	*cmd_1;
	var_t	var;

	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->arg = (char **)malloc(sizeof(char *) * 4);
	cmd_1->arg[0] = ft_strdup("export");
	cmd_1->arg[1] = ft_strjoin("OLDPWD=", getenv("PWD"));
	cmd_1->arg[2] = ft_strjoin("PWD=", getcwd(var.buffer, 200));
	cmd_1->arg[3] = NULL;
	cmd_1->next = NULL;
	export(cmd_1);
	del_cmd(cmd_1, &var);
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
			manevr();
	}
	else
	{
		if (chdir(*(cmd->arg)) == -1)
			perror(strerror(errno));
		else
			manevr();
	}
	return (0);
}

void echom(cmd_t *cmd)
{
	var_t	var;

	var.iter_i = 0;
	if (cmd->arg[1] && !ft_strncmp(cmd->arg[1], "-", 1))
		while (cmd->arg[1][++var.iter_i])
			if(ft_strncmp(cmd->arg[1] + var.iter_i, "n", 1))
				break;
	if (cmd->arg[1] && !cmd->arg[1][var.iter_i])
	{
		var.iter_i = 2;
		while (cmd->arg[var.iter_i] && printf("%s", cmd->arg[var.iter_i]))
		{
			if (cmd->arg[++var.iter_i])
				printf(" ");
		}
	}
	else
	{
		var.iter_i = 1;
		while (cmd->arg[var.iter_i] && printf("%s", cmd->arg[var.iter_i]))
		{
			if (cmd->arg[++var.iter_i])
				printf(" ");
		}
		printf("\n");
	}
}

void	env()
{
	var_t	var;

	var.iter_i = 0;
	while (environ[var.iter_i])
		printf("%s\n", environ[var.iter_i++]);
}

void exitm(cmd_t *cmd)
{
	printf("exit\n");
	if (cmd->arg[1])
	{
		if (cmd->arg[2])
		{
			write(2, "msh: exit: too many arguments\n", 30);
			errno = 1;
			return ;
		}
		exit(ft_atoi(cmd->arg[1]));
	}
	exit(0);
}

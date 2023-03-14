/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:58:47 by aeloyan           #+#    #+#             */
/*   Updated: 2023/03/12 19:02:01 by tumolabs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset(cmd_t *cmd, char **envp)
{
	while (*(++cmd->arg))
	{
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

void	export(cmd_t *cmd, char **envrp)
{
	char	**envp;
	char	**temp;

	temp = cmd->arg;
	while (*(++temp))
	{
		envp = envrp;
		while (*envp)
		{
			if (!ft_strncmp(*envp, *temp, ft_strchr(*temp, '=') - *temp))
			{
				*envp = ft_strdup(*temp);//jshtel, ardyoq petq e malloq unena aystegh
				break ;
			}
			envp++;
		}
		if (!*envp)
		{
			*envp = ft_strdup(*temp);//jshtel, ardyoq petq e malloq unena aystegh
			*(++envp) = NULL;
		}
	}
}

void	pwd()
{
	var_t	var;

	printf("%s\n", getcwd(var.buffer, 200));
}

void	manevr(char **envp)
{
	cmd_t	*cmd_1;
	var_t	var;

	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->arg = (char **)malloc(sizeof(char *) * 4);
	cmd_1->arg[0] = ft_strdup("export");
	cmd_1->arg[1] = ft_strjoin("PWD=", getcwd(var.buffer, 200));
	cmd_1->arg[2] = ft_strjoin("OLDPWD=", getenv("PWD"));
	cmd_1->arg[3] = NULL;
	cmd_1->hd = 0;
	cmd_1->next = NULL;
	export(cmd_1, envp);
	del_cmd(cmd_1, &var);
}

int cd(cmd_t *cmd, char **envp)
{
	if (!*(++cmd->arg))
	{
		cmd->arg--;
		if (!getenv("HOME") || chdir(getenv("HOME")) == -1)
		{
			perror("msh: cd");
		}
		else
			manevr(envp);
	}
	else
	{
		if (chdir(*(cmd->arg--)) == -1)
			perror(strerror(errno));
		else
			manevr(envp);
	}
	return (0);
}

void echom(cmd_t *cmd)
{
	var_t	var;
	int		nline;

	var.iter_i = 0;
	var.iter_j = 0;
	nline = 1;
	while (cmd->arg[++var.iter_j] && !ft_strncmp(cmd->arg[var.iter_j], "-", 1))
	{
		var.iter_i = 0;
		while (cmd->arg[var.iter_j][++var.iter_i] &&
				cmd->arg[var.iter_j][var.iter_i] == 'n')
			;
		if (cmd->arg[var.iter_j][var.iter_i] != '\0')
			break;
		else
			nline = 0;
	}
	if (cmd->arg[var.iter_j])
	{
		while (cmd->arg[var.iter_j] && printf("%s", cmd->arg[var.iter_j]))
		{
			if (cmd->arg[++var.iter_j])
				printf(" ");
		}
	}
	else
	{
		while (cmd->arg[var.iter_j] && printf("%s", cmd->arg[var.iter_j]))
		{
			if (cmd->arg[++var.iter_j])
				printf(" ");
		}
	}
	if (nline)
		printf("\n");
}

void	env(char **envp)
{
	while (*envp)
		printf("%s\n", *envp++);
}

void exitm(cmd_t *cmd)
{
	var_t	var;

	printf("exit\n");
	if (cmd->arg[1])
	{
		var.iter_i = 0;
		while (cmd->arg[1][var.iter_i])
		{
			if (var.iter_i == 0 && (cmd->arg[1][0] == '-' || cmd->arg[1][0] == '+'))
			{
				var.iter_i++;
				continue ;
			}
			if (!ft_isdigit(cmd->arg[1][var.iter_i]))
			{
				write(2, "msh: exit: ", 11);
				write(2, cmd->arg[1], ft_strlen(cmd->arg[1]));
				write(2, ": numeric argument required\n", 28);
				exit(255);
			}	
			var.iter_i++;
		}
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:57:25 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:05:30 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	valid_identifier(char *arg)
{
	int		i;

	i = -1;
	if (arg && arg[0] == '=')
	{
		return (0);
	}
	while (arg && arg[++i] && arg[i] != '=')
		if ((i == 0 && (!ft_isalpha(arg[i]) && arg[i] != '_'))
			|| ((!ft_isalnum(arg[i]) && arg[i] != '_' && arg[i] != '+')
				|| (arg[i] == '+' && arg[i + 1] != '=')))
			return (0);
	return (1);
}

void	unset(t_cmd *cmd, char **e)
{
	char	**temp;

	temp = cmd->arg;
	while (*(++temp))
	{
		if (!valid_identifier(*temp) || ft_strchr(*temp, '='))
		{
			errno = 1;
			ft_putstr_fd("msh: unset: `", 2);
			ft_putstr_fd(*temp, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
			continue ;
		}
		while (*e)
		{
			if (!ft_strncmp(*e, *temp, ft_strchr(*e, '=') - *e))
			{
				free(*(e--));
				while (*(++e))
					*e = e[1];
				break ;
			}
			e++;
		}
	}
}

void	print_export_formatted_env(char **env)
{
	char	*key;
	char	*value;

	while (env && *env)
	{
		if (ft_strchr(*env, '='))
		{
			key = ft_substr(*env, 0, ft_strchr(*env, '=') - *env + 1);
			value = ft_strchr(*env, '=') + 1;
			printf("declare -x %s\"%s\"\n", key, value);
			free(key);
		}
		else
			printf("declare -x %s\n", *env);
		++env;
	}
}

void	manevr(char **envp)
{
	t_cmd	*cmd_1;
	t_var	var;

	cmd_1 = (t_cmd *)malloc(sizeof(t_cmd));
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

int	cd(t_cmd *cmd, char **envp)
{
	if (!*(++cmd->arg))
	{
		cmd->arg--;
		if (!getenv("HOME") || chdir(getenv("HOME")) == -1)
			ft_putstr_fd("msh: cd: HOME not set\n", 2);
		else
			manevr(envp);
	}
	else
	{
		if (chdir(*(cmd->arg--)) == -1)
		{
			ft_putstr_fd("msh: cd: ", 2);
			perror(cmd->arg[1]);
		}
		else
			manevr(envp);
	}
	return (0);
}

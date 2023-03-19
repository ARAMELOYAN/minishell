/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buildins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/10 15:58:47 by aeloyan           #+#    #+#             */
/*   Updated: 2023/03/19 16:36:24 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	valid_identifier(char *arg, char **envp)
{
	int		i;

	i = -1;
	if (arg && arg[0] == '=')
		return (0);
	while (arg && arg[++i] && arg[i] != '=')
		if ((i == 0 && (!ft_isalpha(arg[i]) && arg[i] != '_'))
			|| ((!ft_isalnum(arg[i]) && arg[i] != '_' && arg[i] != '+')
			|| (arg[i] == '+' && arg[i + 1] != '=')))
			return (0);
}

void	unset(cmd_t *cmd, char **e)
{
	char	**temp;

	temp = cmd->arg;
	while (*(++temp))
	{
	if (!valid_identifier(*temp, e) || ft_strchr(*temp, '='))
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
				e--;
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

int	valid(char *arg, char **envp)
{
	char	*ptr;

	if (!valid_identifier(arg, envp))
		return (0);
	while (*envp)
	{
		ptr = ft_strchr(arg, '+');
		if (!ptr)
			ptr = ft_strchr(arg, '=');
		if (ptr)
		{
			if (!ft_strncmp(*envp, arg, ptr - arg))
			{
				if (*ptr == '+')
				{
					if (!ft_strchr(*envp, '='))
						*envp = ft_strjoin(*envp, "=");
					*envp = ft_strjoin(*envp, ptr + 2);
				}
				else 
					*envp = ft_strdup(arg);
				break ;
			}
		}
		else if (!ft_strncmp(*envp, arg, ft_strlen(arg)))
			return (1);
		++envp;
	}
	if (!*envp)
	{
		if (ptr && *ptr == '+')
			ft_memmove(ptr, ptr + 1, ft_strlen(ptr));
		*envp = ft_strdup(arg);
		*(++envp) = NULL;
	}
	return (1);
}

void	export(cmd_t *cmd, char **envp)
{
	char	**temp;

	temp = cmd->arg;
	if (!*(++temp))
		print_export_formatted_env(envp);
	while (*(temp))
	{
		if (!valid(*temp, envp))
		{
			errno = 1;
			ft_putstr_fd("msh: export: `", 2);
			ft_putstr_fd(*temp, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
		}
		++temp;
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
			ft_putstr_fd("msh: cd: HOME not set\n", 2);
		}
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

void echom(cmd_t *cmd)
{
	var_t	var;
	int		nline;

	var.iter_i = 0;
	var.iter_j = 0;
	nline = 1;
	while (cmd->arg[++var.iter_j]
		&& !ft_strncmp(cmd->arg[var.iter_j], "-", 1))
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
		if (ft_strchr(*envp, '='))
			printf("%s\n", *envp++);
		else
		{
			++envp;
			continue ;
		}
}

void	print_err_arg(const char *s)
{
	ft_putstr_fd("msh: exit: ", 2);
	ft_putstr_fd((char *)s, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	exit(255);
}	

long int	msh_atoi(const char *s)
{
	unsigned long	val;
	int		i;
	int		k;

	val = 0;
	i = 0;
	k = 1;
	while (s[i] == ' ' || s[i] == '-' || s[i] == '+')
		if ((s[i++] == '+' || s[i - 1] == '-') && !ft_isdigit(s[i]))
			print_err_arg(s);
	if (i != 0 && s[i - 1] == '-')
		k = -1;
	while (ft_isdigit(s[i]))
	{
		val = (val * 10) + s[i++] - '0';
		if (val &&  ((val > LONG_MAX && k == 1) || -val < LONG_MIN))
			print_err_arg(s);
	}
	return (val * k);
}

void exitm(cmd_t *cmd)
{
	var_t	var;
	long	val;

	printf("exit\n");
	if (cmd->arg[1])
	{
		var.iter_i = 0;
		while (cmd->arg[1][var.iter_i])
		{
			if (var.iter_i == 0 && (cmd->arg[1][0] == '-'
				|| cmd->arg[1][0] == '+'))
				;
			else if (!ft_isdigit(cmd->arg[1][var.iter_i]))
				print_err_arg(cmd->arg[1]);
			var.iter_i++;
		}
		val = msh_atoi(cmd->arg[1]); 
		if (cmd->arg[2])
		{
			ft_putstr_fd("msh: exit: too many arguments\n", 2);
			errno = 1;
			return ;
		}
		exit(val);
	}
	exit(0);
}

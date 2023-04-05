/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:54:33 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:07:45 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	not_coresp(char ***envp, char *ptr, char *arg)
{
	if (ptr - arg != ft_strchr(**envp, '=') - **envp)
	{
		(*envp)++;
		return (1);
	}
	return (0);
}

int	add_envi_mej(char ***envp, char *temp, char *ptr, char *arg)
{
	if (*ptr == '+')
	{
		if (!ft_strchr(**envp, '='))
		{
			temp = **envp;
			**envp = ft_strjoin(**envp, "=");
			free(temp);
		}
		else if (not_coresp(envp, ptr, arg))
			return (0);
		temp = **envp;
		**envp = ft_strjoin(**envp, ptr + 2);
		free(temp);
	}
	else
	{
		if (not_coresp(envp, ptr, arg))
			return (0);
		temp = **envp;
		**envp = ft_strdup(arg);
		free(temp);
	}
	return (1);
}

int	search_in_env(char ***envp, char **ptr, char *arg)
{
	char	*temp;

	temp = NULL;
	*ptr = ft_strchr(arg, '+');
	if (!*ptr)
		*ptr = ft_strchr(arg, '=');
	if (*ptr)
	{
		if (!ft_strncmp(**envp, arg, *ptr - arg))
		{
			if (!add_envi_mej(envp, temp, *ptr, arg))
				return (0);
			return (2);
		}
	}
	else if (!ft_strncmp(**envp, arg, ft_strlen(arg)))
		return (1);
	++(*envp);
	return (3);
}

int	valid(char *arg, char **envp)
{
	char	*ptr;
	char	*temp;
	int		d;

	if (!valid_identifier(arg))
		return (0);
	while (*envp)
	{
		d = search_in_env(&envp, &ptr, arg);
		if (!d)
			continue ;
		if (d == 1)
			return (1);
		if (d == 2)
			break ;
	}
	if (*envp)
		return (1);
	if (ptr && *ptr == '+')
		ft_memmove(ptr, ptr + 1, ft_strlen(ptr));
	temp = *envp;
	*envp = ft_strdup(arg);
	free(temp);
	*(++envp) = NULL;
	return (1);
}

void	export(t_cmd *cmd, char **envp)
{
	char	**temp;

	if (!cmd)
		return ;
	temp = cmd->arg;
	if (!*(++temp))
		print_export_formatted_env(envp);
	while (*(temp))
	{
		if (!valid(*temp, envp))
		{
			g_status = 1;
			ft_putstr_fd("msh: export: `", 2);
			ft_putstr_fd(*temp, 2);
			ft_putstr_fd("': not a valid identifier\n", 2);
		}
		++temp;
	}
}

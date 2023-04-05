/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_and_run.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:10:23 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:09:58 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	dev_cmd(char **s, t_cmd **cmd, t_var *var)
{
	char	*dst;

	if (var->ptr == *s || *(++var->ptr) == '|')
	{
		g_status = 258;
		return (0);
	}
	dst = (char *)malloc(var->ptr - *s);
	if (!dst)
	{
		perror("msh");
		g_status = 1;
		return (0);
	}
	ft_strlcpy(dst, *s, var->ptr - *s);
	if (!add_cmd(dst, cmd, var))
	{
		free(dst);
		return (0);
	}
	free(dst);
	*s = var->ptr;
	var->ptr = ft_strchr(*s, '|');
	return (1);
}

int	bef_quo(t_cmd **cmd, char **s, t_quote **quote, t_var *var)
{
	t_quote	*quote_1;

	while (var->ptr && var->ptr < (*quote)->start)
	{
		if (!dev_cmd(s, cmd, var))
		{
			free(*quote);
			return (0);
		}
	}
	if (var->ptr)
		var->ptr = ft_strchr((*quote)->end, '|');
	quote_1 = *quote;
	*quote = get_quote((*quote)->end + 1);
	free(quote_1);
	return (1);
}

t_quote	*get_quote(char *ptr)
{
	t_quote	*quote;

	if (!ptr)
		return ((t_quote *)2);
	quote = (t_quote *)malloc(sizeof(t_quote));
	if (!quote)
		return ((t_quote *)1);
	quote->start = ft_strchr(ptr, '\'');
	quote->end = ft_strchr(ptr, '"');
	if ((quote->start || quote->end))
	{
		if (quote->start && (quote->start < quote->end || !quote->end))
			quote->end = ft_strchr(quote->start + 1, '\'');
		else
		{
			quote->start = quote->end;
			quote->end = ft_strchr(quote->start + 1, '"');
		}
		if (quote->end)
			return (quote);
		free(quote);
		return ((t_quote *)3);
	}
	free(quote);
	return ((t_quote *)0);
}

int	check_syntax_error(char *s, t_cmd **cmd, t_var *var)
{
	t_quote	*quote;

	*cmd = NULL;
	var->count = 0;
	var->ptr = ft_strchr(s, '|');
	quote = get_quote(s);
	if (quote == (t_quote *)1)
		return (0);
	while (quote > (t_quote *)3 && var->ptr)
		if (!bef_quo(cmd, &s, &quote, var))
			return (0);
	if (quote == (t_quote *)3)
	{
		ft_putstr_fd("msh: syntax error near unexpected token `quote'\n", 2);
		g_status = 258;
		return (0);
	}
	if (quote > (t_quote *)3)
		free(quote);
	while (var->ptr)
		if (!dev_cmd(&s, cmd, var))
			return (0);
	return (add_cmd(s, cmd, var));
}

void	parse_and_run(t_cmd *cmd, char *ch, t_var *var, char **envp)
{
	add_history(ch);
	if (check_syntax_error(ch, &cmd, var))
	{
		run(cmd, var, envp);
		while (var->count--)
			wait(NULL);
	}
}

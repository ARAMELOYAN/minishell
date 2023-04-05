/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_and_run.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:10:23 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:22:18 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	dev_cmd(char **s, cmd_t **cmd, var_t *var)
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

int	bef_quo(cmd_t **cmd, char **s, quote_t **quote, var_t *var)
{
	quote_t	*quote_1;

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

quote_t	*get_quote(char *ptr)
{
	quote_t	*quote;

	if (!ptr)
		return ((quote_t *)2);
	quote = (quote_t *)malloc(sizeof(quote_t));
	if (!quote)
		return ((quote_t *)1);
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
		return ((quote_t *)3);
	}
	free(quote);
	return ((quote_t *)0);
}

int	check_syntax_error(char *s, cmd_t **cmd, var_t *var)
{
	quote_t	*quote;

	*cmd = NULL;
	var->count = 0;
	var->ptr = ft_strchr(s, '|');
	quote = get_quote(s);
	if (quote == (quote_t *)1)
		return (0);
	while (quote > (quote_t *)3 && var->ptr)
		if (!bef_quo(cmd, &s, &quote, var))
			return (0);
	if (quote == (quote_t *)3)
	{
		ft_putstr_fd("msh: syntax error near unexpected token `quote'\n", 2);
		g_status = 258;
		return (0);
	}
	if (quote > (quote_t *)3)
		free(quote);
	while (var->ptr)
		if (!dev_cmd(&s, cmd, var))
			return (0);
	return (add_cmd(s, cmd, var));
}

void	parse_and_run(cmd_t *cmd, char *ch, var_t *var, char **envp)
{
	add_history(ch);
	if (check_syntax_error(ch, &cmd, var))
	{
		run(cmd, var, envp);
		while (var->count--)
			wait(NULL);
	}
}

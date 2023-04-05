/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_name_finding.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:34:24 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:36:31 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	redir_input(cmd_t *cmd, var_t *var, int i)
{
	if (i == 0)
		var->fd = open(var->file, O_RDONLY);
	else
		heredoc(cmd, var);
	free(var->file);
	if (var->fd == -1)
		return (1);
	if (cmd->fd[0] > 0)
		close(cmd->fd[0]);
	cmd->fd[0] = var->fd;
	return (0);
}

int	redir_output(cmd_t *cmd, var_t *var, int i)
{
	if (i > 7)
		var->fd = open(var->file, O_RDWR | O_CREAT | O_APPEND, 0644);
	else
		var->fd = open(var->file, O_RDWR | O_CREAT | O_TRUNC, 0644);
	free(var->file);
	if (var->fd == -1)
		return (1);
	if (var->redir_error)
	{
		if (cmd->fd[2] > 2)
			close(cmd->fd[2]);
		cmd->fd[2] = var->fd;
	}
	else
	{
		if (cmd->fd[1] > 1)
			close(cmd->fd[1]);
		cmd->fd[1] = var->fd;
	}
	return (0);
}

int	special(char *ch)
{
	if (*ch == '|' || *ch == '&' || *ch == ';' || *ch == '<' || *ch == '>'
		|| *ch == '(' || *ch == ')' || *ch == '$' || *ch == '`'
		|| *ch == '"' || *ch == '\0' || *ch == '\'' || *ch == ' ')
		return (1);
	return (0);
}

char	*next_special(char *str)
{
	if (!str || !*str)
		return (0);
	while (!special((++str)))
		;
	return (str);
}

void	find_key(char *str, var_t *var)
{
	quote_t	*quote;
	quote_t	*quote_1;

	var->ptr = next_special(str);
	quote = get_quote(str);
	var->open_dollar = 1;
	while (quote > (quote_t *)3 && var->ptr >= quote->start)
	{
		var->open_dollar = 0;
		var->ptr = next_special(quote->end);
		quote_1 = quote;
		quote = get_quote(quote->end + 1);
		free(quote_1);
	}
	if (quote > (quote_t *)3)
		free(quote);
}

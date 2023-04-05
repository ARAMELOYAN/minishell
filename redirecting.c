/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirecting.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:30:25 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:10:14 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	clear_quote(char **arg)
{
	t_quote	*quote;
	t_quote	*quote_1;

	while (arg && *arg)
	{
		quote = get_quote(*arg);
		while (quote > (t_quote *)3)
		{
			ft_memmove(quote->start, quote->start + 1, ft_strlen(quote->start));
			ft_memmove(quote->end - 1, quote->end, ft_strlen(quote->end) + 1);
			quote_1 = quote;
			quote = get_quote(quote->end - 1);
			free(quote_1);
		}
		arg++;
	}
}

int	my_open(t_cmd *cmd, char *str, char *motiv, int red)
{
	t_var	var;
	char	*file[2];

	var.envp = NULL;
	var.redir_error = red % 2;
	file[1] = NULL;
	if (str && *str)
	{
		var.file_1 = ft_strtrim(str, " \t\v\n");
		find_key(str, &var);
		ft_memmove(str, var.ptr, ft_strlen(var.ptr) + 1);
		find_key(var.file_1, &var);
		*(var.ptr) = '\0';
		var.file = ft_strtrim(var.file_1, " \t\v\n");
		free(var.file_1);
		file[0] = var.file;
		clear_quote(file);
		if (!ft_strncmp(motiv, "output", 7))
			return (redir_output(cmd, &var, red));
		else if (!ft_strncmp(motiv, "input", 6))
			return (redir_input(cmd, &var, red));
		return (0);
	}
	return (0);
}

int	syntax_error(char **ptr, char ch)
{
	ft_memmove(*ptr, *ptr + 1, ft_strlen(*ptr));
	if ((*(*ptr) == ch && special(*ptr + 1) && *(*ptr + 1) != ' '
			&& *(*ptr + 1) != '"' && *(*ptr + 1) != '\'')
		|| (*(*ptr) != ch && special(*ptr) && **ptr != ' '
			&& *(*ptr) != '"' && *(*ptr) != '\'')
		|| (**ptr == ' ' && special(*ptr + 1)
			&& *(*ptr + 1) != '"' && *(*ptr + 1) != '\''))
	{
		ft_putstr_fd("msh: syntax error near unexpected token `", 2);
		if (!**ptr)
			ft_putstr_fd("newline", 2);
		ft_putchar_fd(*(++*ptr), 2);
		ft_putstr_fd("'\n", 2);
		return (1);
	}
	else if (!*(*ptr) || !*(*ptr + 1))
	{
		perror("SPECIAL 2");
		return (1);
	}
	return (0);
}

int	set_redir_error(t_quote **quote, char **ptr, char ch)
{
	t_quote	*quote_1;

	while (*quote > (t_quote *)3 && *ptr > (*quote)->start)
	{
		*ptr = ft_strchr((*quote)->end, ch);
		quote_1 = *quote;
		*quote = get_quote((*quote)->end + 1);
		free(quote_1);
	}
	if (*quote > (t_quote *)3)
		free(*quote);
	if (!*ptr)
		return (-1);
	if (*ptr && *(*ptr - 1) == '2')
	{
		ft_memmove(*ptr - 1, *ptr, ft_strlen(*ptr) + 1);
		if (*(--*ptr) == '>')
			return (1);
		return (0);
	}
	return (0);
}

int	parse_str(t_cmd *cmd, char *str, char *motiv, char ch)
{
	t_var	var;
	t_quote	*quote;

	var.ptr = ft_strchr(str, ch);
	quote = get_quote(str);
	while (var.ptr)
	{
		var.redir_error = set_redir_error(&quote, &var.ptr, ch);
		if (var.redir_error == -1)
			break ;
		if (syntax_error(&var.ptr, ch))
			return (0);
		if (*(var.ptr) == ch)
		{
			ft_memmove(var.ptr, var.ptr + 1, ft_strlen(var.ptr));
			my_open(cmd, var.ptr, motiv, var.redir_error + 8);
		}
		else
			my_open(cmd, var.ptr, motiv, var.redir_error);
		var.ptr = ft_strchr(str, ch);
		quote = get_quote(str);
	}
	if (quote > (t_quote *)3)
		free(quote);
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_sign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:23:19 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:06:03 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*next_word(char *str)
{
	int	i;

	i = 0;
	if (*str == '?')
		return (ft_substr(str, 0, 1));
	while (ft_isalnum(str[i]) || str[i] == '_')
			i++;
	return (ft_substr(str, 0, i));
}

void	replace(char **arg, char **ptr, char **envp)
{
	char	*temp;
	char	*word;
	char	*arg_1;
	char	*err;

	word = next_word(*ptr + 1);
	err = ft_itoa(g_status);
	**ptr = '\0';
	if (*(*ptr + 1) == '?')
		arg_1 = ft_strjoin(*arg, err);
	else if (my_getenv(envp, word))
		arg_1 = ft_strjoin(*arg, my_getenv(envp, word));
	else
		arg_1 = ft_strdup(*arg);
	temp = ft_strjoin(arg_1, *ptr + ft_strlen(word) + 1);
	free(*arg);
	*arg = temp;
	free(err);
	free(arg_1);
	free(word);
	*ptr = ft_strchr(*arg, '$');
	while (*ptr && !ft_isalpha(*(*ptr + 1)) && *(*ptr + 1) != '?')
		*ptr = ft_strchr(*ptr + 1, '$');
}

void	replace_dollar(char **arg, t_quote **quote, char **ptr, char **envp)
{
	t_quote	*quote_1;

	if (ft_isalpha(*(*ptr + 1)) || *(*ptr + 1) == '?' || *(*ptr + 1) == '_')
	{
		replace(arg, ptr, envp);
		if (quote && *quote > (t_quote *)3)
		{
			free(*quote);
			*quote = get_quote(*arg);
		}
		while (quote && *quote > (t_quote *)3 && (*quote)->end < *ptr)
		{
			quote_1 = *quote;
			*quote = get_quote((*quote)->end + 1);
			free(quote_1);
		}
	}
	else
		*ptr = ft_strchr(*ptr + 1, '$');
}

void	dollar_into_quote(char **ptr, char **arg, char **envp, t_quote **quote)
{
	t_quote	*quote_1;

	while (*ptr && (*quote) > (t_quote *)3 && *ptr < (*quote)->start)
		replace_dollar(arg, quote, ptr, envp);
	while (*ptr && (*quote) > (t_quote *)3 && *ptr > (*quote)->start
		&& *ptr < (*quote)->end && *(*quote)->start == '"')
		replace_dollar(arg, quote, ptr, envp);
	if ((*quote) > (t_quote *)3 && *(*quote)->end == '\'')
	{
		*ptr = ft_strchr((*quote)->end, '$');
		quote_1 = (*quote);
		(*quote) = get_quote((*quote)->end + 1);
		free(quote_1);
	}
	while (*ptr && (*quote) > (t_quote *)3 && *ptr > (*quote)->end)
	{
		quote_1 = (*quote);
		(*quote) = get_quote((*quote)->end + 1);
		free(quote_1);
	}
}

void	find_dollar(char **arg, char **envp)
{
	t_quote	*quote;
	char	*ptr;

	while (arg && *arg)
	{
		ptr = ft_strchr(*arg, '$');
		quote = get_quote(*arg);
		while (ptr && quote > (t_quote *)3)
			dollar_into_quote(&ptr, arg, envp, &quote);
		while (ptr)
			replace_dollar(arg, NULL, &ptr, envp);
		if (quote > (t_quote *)3)
			free(quote);
		arg++;
	}
}

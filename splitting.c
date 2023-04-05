/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   splitting.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:37:03 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:38:43 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	init_val(quote_t **quote, int *i, char **p_e, char *s)
{
	*i = 0;
	*p_e = s;
	*quote = get_quote(s);
	return (0);
}

int	search_word(char **p_s, char *s, char c, int i)
{
	while (s[i] == c)
		*p_s = &s[++i];
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

int	search_end(quote_t **qu, char **p_e, char *s)
{
	quote_t	*quote_1;

	*p_e = my_min(ft_strchr((*qu)->end, ' '), ft_strchr((*qu)->end, '\0'));
	quote_1 = *qu;
	*qu = get_quote((*qu)->end + 1);
	free(quote_1);
	return (*p_e - s);
}

char	**splitting(char **mat, char *s, char c)
{
	quote_t	*quote;
	char	*p_start;
	char	*p_end;
	int		i;
	int		j;

	j = init_val(&quote, &i, &p_end, s);
	p_start = s;
	while (s[i])
	{
		i = search_word(&p_start, s, c, i);
		p_end = s + i;
		while (quote > (quote_t *)3 && p_end > quote->start)
			i = search_end(&quote, &p_end, s);
		if (p_start < p_end)
			mat[j] = ft_substr(s, p_start - s, p_end - p_start);
		if (p_start < p_end && !mat[j++])
		{
			clean(mat, j);
			return (0);
		}
	}
	mat[j] = 0;
	return (mat);
}

char	**my_split(char const *s, char c)
{
	char	**mat;

	if (!s)
		return (0);
	mat = (char **)malloc(sizeof(char *) * (rows((char *)s, c) + 1));
	if (!mat)
		return (0);
	mat = splitting(mat, (char *)s, c);
	if (!mat)
		return (0);
	return (mat);
}

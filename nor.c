typedef sturct quote
{
	char	*start;
	char	*end;
}	quote_t;

quote_t	*get_quote(char *ptr)
{
	quote_t quote;

	if (!ptr)
		return (0);
	quote.start = ft_strchr(ptr, ''');
	quote.end = ft_strchr(ptr, '"');
	if (quote.start < quote.end)
		quote.end = ft_strchr(quote.start, ''');
	else
	{
		quote.start = quote.end;
		quote.end = ft_strchr(quote.start, '"');
	}
	return (quote);
}

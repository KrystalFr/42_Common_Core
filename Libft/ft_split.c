/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 15:49:59 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/09 18:53:16 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count(char const *str, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	if (str[0] != c && str[0] != '\0')
		count++;
	while (str[i])
	{
		if (str[i] == c && (str[i + 1] != c && str[i + 1] != '\0'))
			count++;
		i++;
	}
	return (count);
}

static char	*ft_strndup(char *src, int len)
{
	char	*dup;
	int		i;

	dup = malloc((len + 1) * sizeof(char));
	if (!dup)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = src[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		j;
	int		gap;
	char	**tab;

	if (!s)
		return (NULL);
	tab = malloc((ft_count(s, c) + 1) * sizeof(char *));
	if (!tab)
		return (NULL);
	j = -1;
	i = 0;
	while (++j < ft_count(s, c))
	{
		gap = 0;
		while (s[i] == c)
			i++;
		i--;
		while (s[++i] != c && s[i])
			gap++;
		tab[j] = ft_strndup((char *)&s[i - gap], gap);
	}
	tab[j] = 0;
	return (tab);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/01 09:38:54 by gaperaud          #+#    #+#             */
/*   Updated: 2023/12/01 12:32:49 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_countwords(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] == c)
		{
			while (s[i] == c)
				i++;
		}
		else
		{
			count++;
			while (s[i] != c && s[i])
				i++;
		}
	}
	return (count);
}

static char	*get_nextword(char const *str, int *ptr, char c)
{
	int		i;
	int		len_word;
	char	*word;

	i = 0;
	len_word = 0;
	while (str[*ptr] && str[*ptr] == c)
		*ptr = *ptr + 1;
	if (!str[*ptr])
		return (NULL);
	while (str[*ptr + len_word] && str[*ptr + len_word] != c)
		len_word++;
	word = malloc((len_word + 1) * sizeof(char));
	if (!word)
		return (NULL);
	while (i < len_word)
	{
		word[i] = str[*ptr];
		i++;
		*ptr = *ptr + 1;
	}
	word[i] = 0;
	return (word);
}

char	**ft_split(char const *s, char c)
{
	int		count;
	int		i;
	int		j;
	char	**tab;

	i = 0;
	j = 0;
	if (!s)
		return (NULL);
	count = ft_countwords(s, c);
	tab = (char **)malloc(sizeof(char *) * (count + 1));
	if (!tab)
		return (NULL);
	while (i < count)
	{
		tab[i] = get_nextword(s, &j, c);
		if (!tab[i])
			return (NULL);
		i++;
	}
	tab[i] = 0;
	return (tab);
}

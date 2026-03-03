/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:02:34 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/22 17:10:01 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_bzero(void *s, size_t n)
{
	char	*tab;

	tab = s;
	while (n--)
		tab[n] = 0;
}

int	ft_strchr(char *str, char c)
{
	size_t	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strjoin(char *s1, char *s2)
{
	int		i;
	int		j;
	char	*tmp;

	tmp = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!tmp)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		tmp[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		tmp[i] = s2[j];
		i++;
		j++;
	}
	tmp[i] = '\0';
	free(s1);
	return (tmp);
}

char	*ft_strdup(char *src)
{
	int		i;
	char	*tab;

	i = 0;
	tab = malloc((ft_strlen(src) + 1) * sizeof(char));
	if (!tab)
		return (NULL);
	while (src[i])
	{
		tab[i] = src[i];
		i++;
	}
	tab[i] = '\0';
	return (tab);
}

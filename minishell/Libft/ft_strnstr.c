/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 06:55:04 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/14 17:38:15 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *str);

char	*ft_strnstr(const char *big, const char *little, size_t n)
{
	size_t	i;
	size_t	j;

	if (big == 0 && n == 0)
		return (0);
	i = 0;
	if (!little[0])
		return ((char *)&big[0]);
	while (big[i] && i < n)
	{
		j = 0;
		while (little[j] && big[i + j] == little[j] && (i + j) < n)
			j++;
		if (j == ft_strlen(little))
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}

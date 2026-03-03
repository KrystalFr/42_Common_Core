/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 07:02:21 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 16:32:03 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	size_t		i;
	char		*str;
	int			comp;

	i = ft_strlen(s1) + ft_strlen(s2);
	comp = 0;
	str = malloc(i + 1);
	i = 0;
	if (!str)
		return (NULL);
	while (s1 && s1[comp])
	{
		str[i] = s1[comp];
		i++;
		comp++;
	}
	comp = 0;
	while (s2 && s2[comp])
	{
		str[i] = s2[comp];
		i++;
		comp++;
	}
	str[i] = 0;
	return (str);
}

// int	main(void)
// {
// 	printf("%s\n", ft_strjoin("salut", " a tous"));
// 	return (0);
// }

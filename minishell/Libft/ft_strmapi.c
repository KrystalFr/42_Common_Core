/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leG <leG@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 06:12:34 by leG               #+#    #+#             */
/*   Updated: 2023/11/14 10:22:51 by leG              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *str);

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*dest;
	int		i;

	if (!s || !f)
		return (NULL);
	dest = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (!dest)
		return (NULL);
	i = 0;
	while (s[i])
	{
		dest[i] = ((*f)(i, s[i]));
		i++;
	}
	dest[i] = 0;
	return (dest);
}

// typedef char (*f)(unsigned int, char);

// char	ft_test(unsigned int i, char a)
// {
// 	a += i;
// 	return (a);
// }

// int	main(void)
// {
// 	f ptr = ft_test;

// 	printf("%s\n", ft_strmapi("ayo", ptr));
// 	return (0);
// }
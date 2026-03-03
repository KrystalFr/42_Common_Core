/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 12:02:32 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/14 12:27:56 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;
	char	*s1;

	i = 0;
	s1 = (char *)dst;
	if (!dst && !src)
		return (NULL);
	while (i < n)
	{
		*(char *)s1 = *(char *)src;
		s1++;
		src++;
		i++;
	}
	return (dst);
}

// int main(void)
// {
//     char str1[20] = "Hello, World!";
//     char str2[20];
//     char str3[20];

//     ft_memcpy(str2, str1, strlen(str1) + 1);
//     memcpy(str3, str1, strlen(str1) + 1);
//     printf("str1: %s\n", str1);
//     printf("str2: %s\n", str2);
//     printf("str3: %s\n", str3);

//     return (0);
// }

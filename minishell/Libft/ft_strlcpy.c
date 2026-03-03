/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 23:20:33 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/14 12:47:04 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	unsigned int	i;

	i = 0;
	if (size > 0)
	{
		while (--size && src[i])
		{
			dest[i] = src[i];
			i++;
		}
		dest[i] = 0;
	}
	while (src[i])
		i++;
	return (i);
}

// int main(void)
// {
//     char src[] = "";
//     char dest[20];

//     size_t len_ft_strlcpy = ft_strlcpy(dest, src, sizeof(dest));
//     printf("dest (ft_strlcpy): %s\n", dest);
//     printf("Length: %zu\n", len_ft_strlcpy);
//     char dest_strlcpy[20];
//     size_t len_strlcpy = strlcpy(dest_strlcpy, src, sizeof(dest_strlcpy));
//     printf("dest (strlcpy): %s\n", dest_strlcpy);
//     printf("Length: %zu\n", len_strlcpy);

//     return (0);
// }

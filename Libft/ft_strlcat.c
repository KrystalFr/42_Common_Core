/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 20:44:33 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/14 16:04:44 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	len;

	i = 0;
	if (!size && !src)
		return (0);
	len = ft_strlen(dest);
	if (len >= size)
		return (size + ft_strlen(src));
	while (src[i] && i < (size - len) - 1)
	{
		dest[len + i] = src[i];
		i++;
	}
	dest[len + i] = '\0';
	return (ft_strlen(src) + len);
}

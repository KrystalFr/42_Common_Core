/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 00:22:04 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/14 16:25:13 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	t_dest;
	size_t	t_src;

	i = 0;
	t_dest = 0;
	t_src = 0;
	while (src[t_src])
		t_src++;
	while (t_dest < size && dest[t_dest])
		t_dest++;
	if (!size)
		return (t_src);
	while ((i + t_dest + 1) < size && src[i])
	{
		dest[t_dest + i] = src[i];
		i++;
	}
	if (t_dest < size)
		dest[t_dest + i] = '\0';
	while (src[i])
		i++;
	return (i + t_dest);
}

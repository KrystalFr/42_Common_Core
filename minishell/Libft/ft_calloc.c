/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leG <leG@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 07:38:47 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/20 15:01:01 by leG              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *str, size_t n);

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*res;
	size_t	nmemb_size;

	nmemb_size = nmemb * size;
	if (nmemb_size == 0)
		return (malloc(1));
	if (nmemb != nmemb_size / size)
		return (NULL);
	res = (void *)malloc(nmemb * size);
	if (!res)
		return (NULL);
	ft_bzero(res, nmemb * size);
	return (res);
}

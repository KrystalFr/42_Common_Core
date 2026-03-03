/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 21:53:24 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/07 23:15:05 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *ptr, int val, size_t size)
{
	size_t	i;
	char	*tab;

	i = 0;
	tab = ptr;
	while (i < size)
	{
		tab[i] = val;
		i++;
	}
	return (ptr);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leG <leG@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 20:30:22 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/13 07:18:03 by leG              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *str, int c, size_t n)
{
	char	*str0;

	str0 = str;
	while (n)
	{
		*str0 = (unsigned char)c;
		str0++;
		n--;
	}
	return (str);
}

// int main(void)
// {
//     char str[10];
//     char str0[10];

//     memset(str, 'a', 9);

//     ft_memset(str0, 'b', 9);

//     printf("str : %s\n", str);
//     printf("str0: %s\n", str0);

//     return (0);
// }
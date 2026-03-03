/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leG <leG@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 21:11:40 by gaperaud          #+#    #+#             */
/*   Updated: 2023/11/13 07:29:59 by leG              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *str, size_t n)
{
	char	*s;
	size_t	i;

	i = 0;
	s = str;
	while (i < n)
	{
		s[i] = 0;
		i++;
	}
}

// #include <unistd.h>
// int main(void)
// {
//     char str[10] = "aaaaaaaa";
//     char str0[10] = "aaaaaaaa";
//     printf("%s, %s\n", str, str0);
//     ft_bzero(str, 5);
//     bzero(str0, 5);
//     int i = 0;
//     write (1, str, 10);
//     printf("\n");
//     write (1, str0, 10);
//     printf("\n");
//     return (0);
// }

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 14:11:52 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/09 18:44:59 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count(int n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count++;
	while (n)
	{
		n /= 10;
		count++;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	char			*tab;
	int				i;
	unsigned int	nb;

	i = ft_count(n);
	tab = malloc((ft_count(n) + 1) * sizeof(char));
	if (!tab)
		return (NULL);
	nb = n;
	if (nb == 0)
		tab[0] = '0';
	if (n < 0)
	{
		tab[0] = '-';
		nb = n * -1;
	}
	tab[i] = '\0';
	while (nb)
	{
		i--;
		tab[i] = nb % 10 + '0';
		nb /= 10;
	}
	return (tab);
}

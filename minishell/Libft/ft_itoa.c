/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 05:25:49 by leG               #+#    #+#             */
/*   Updated: 2025/02/18 22:17:50 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

long	ft_size(long nb)
{
	long	taille;

	taille = 1;
	if (nb == 0)
		return (1);
	while (nb)
	{
		nb /= 10;
		taille *= 10;
	}
	return (taille / 10);
}

int	ft_len(int n)
{
	int		i;
	long	nb;

	i = 0;
	nb = n;
	if (n == 0)
		return (1);
	if (n < 0)
	{
		i++;
		nb *= -1;
	}
	while (nb)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

void	ft_rempl(long taille, long nb, int i, char *dest)
{
	while (taille)
	{
		dest[i] = nb / taille + '0';
		nb %= taille;
		taille /= 10;
		i++;
	}
	dest[i] = '\0';
}

char	*ft_itoa(int n)
{
	char	*dest;
	long	nb;
	int		i;
	long	taille;

	nb = n;
	i = 0;
	dest = (char *)malloc(sizeof(char) * ft_len(n) + 1);
	if (!dest)
		return (NULL);
	taille = ft_size(nb);
	if (nb < 0)
	{
		nb = -nb;
		dest[i] = '-';
		i++;
	}
	ft_rempl(taille, nb, i, dest);
	return (dest);
}

// int	main(void)
// {
// 	printf("%s\n", ft_itoa(-0));
// 	return (0);
// }
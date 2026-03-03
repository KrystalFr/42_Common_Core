/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_put_csn.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 00:48:30 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/19 02:02:31 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putchar(char c, int ret)
{
	write(1, &c, 1);
	ret++;
	return (ret);
}

int	ft_putstr(char *str, int ret)
{
	int	i;

	i = 0;
	if (!str)
	{
		write(1, "(null)", 6);
		ret += 6;
	}
	else
	{
		while (str[i])
		{
			ret++;
			i++;
		}
		write(1, str, i);
	}
	return (ret);
}

int	ft_putnbr(int n, int ret)
{
	unsigned int	nb;

	nb = n;
	if (n < 0)
	{
		ret = ft_putchar('-', ret);
		nb = n * -1;
	}
	if (nb > 9)
		ret = ft_putnbr(nb / 10, ret);
	ret = ft_putchar(nb % 10 + 48, ret);
	return (ret);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_number.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 13:00:29 by leG               #+#    #+#             */
/*   Updated: 2025/01/17 00:32:14 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf_ptr_recurs(size_t n, int *compt)
{
	char	*buff;

	buff = "0123456789abcdef";
	if (n > 15)
		ft_printf_ptr_recurs(n / 16, compt);
	ft_putchar(buff[n % 16]);
	(*compt)++;
	return (*compt);
}

int	ft_printf_ptr(size_t a)
{
	int	compt;

	if (a == 0)
		return (ft_putstr("(nil)"));
	compt = 0;
	write(1, "0x", 2);
	return (ft_printf_ptr_recurs(a, &compt) + 2);
}

int	ft_print_unsigned_dec(int nb)
{
	unsigned int	n;
	int				compt;

	n = nb;
	if (nb < 0)
		compt = ft_taille(4294967296 + nb);
	else
		compt = ft_taille(n);
	if (n > 9)
		ft_print_unsigned_dec(n / 10);
	ft_putchar(n % 10 + '0');
	return (compt);
}

int	ft_printnbr(int n)
{
	int	compt;

	compt = ft_taille(n);
	if (n == -2147483648)
	{
		ft_putstr("-2147483648");
		return (11);
	}
	else
	{
		if (n < 0)
		{
			ft_putchar('-');
			n *= -1;
		}
		if (n > 9)
			ft_printnbr(n / 10);
		ft_putchar(n % 10 + '0');
	}
	return (compt);
}

int	ft_taille(long long int n)
{
	long long	nb;
	int			compt;

	compt = 0;
	nb = n;
	if (n == 0)
		return (1);
	if (n < 0)
	{
		nb *= -1;
		compt++;
	}
	while (nb > 0)
	{
		nb /= 10;
		compt++;
	}
	return (compt);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_hexa.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 12:58:46 by leG               #+#    #+#             */
/*   Updated: 2025/01/17 00:32:29 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_print_lowerhexa_recursive(unsigned int n, int *compt)
{
	char	*buff;

	buff = "0123456789abcdef";
	if (n > 15)
		ft_print_lowerhexa_recursive(n / 16, compt);
	ft_putchar(buff[n % 16]);
	(*compt)++;
	return (*compt);
}

int	ft_print_upperhexa_recursive(unsigned int n, int *compt)
{
	char	*buff;

	buff = "0123456789ABCDEF";
	if (n > 15)
		ft_print_upperhexa_recursive(n / 16, compt);
	ft_putchar(buff[n % 16]);
	(*compt)++;
	return (*compt);
}

int	ft_print_lowerhexa(unsigned int n)
{
	int	compt;

	compt = 0;
	return (ft_print_lowerhexa_recursive(n, &compt));
}

int	ft_print_upperhexa(unsigned int n)
{
	int	compt;

	compt = 0;
	return (ft_print_upperhexa_recursive(n, &compt));
}

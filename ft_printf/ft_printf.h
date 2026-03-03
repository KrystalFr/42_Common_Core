/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/19 00:34:09 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/19 21:48:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdlib.h>
# include <unistd.h>
# include <stdarg.h>
# include <stdio.h>

int	ft_printf(const char *str, ...);
int	ft_putchar(char c, int ret);
int	ft_putstr(char *str, int ret);
int	ft_putnbr(int n, int ret);
int	ft_putunsigned(unsigned int nb, int ret);
int	ft_puthexa(unsigned int nb, char *base, int ret);
int	ft_putpoint(unsigned long long ptr, int ret);
#endif
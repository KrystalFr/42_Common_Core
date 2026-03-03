/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 12:56:46 by leG               #+#    #+#             */
/*   Updated: 2025/01/17 00:32:04 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putchar(char c)
{
	write(1, &c, 1);
	return (1);
}

int	ft_putstr(const char *buffer)
{
	int	i;

	if (buffer == NULL)
	{
		write (1, "(null)", 6);
		return (6);
	}
	i = 0;
	while (buffer[i])
		ft_putchar(buffer[i++]);
	return (i);
}

int	ft_print_percent(void)
{
	ft_putchar('%');
	return (1);
}

int	ft_type(char type, va_list args)
{
	int	len;

	len = 0;
	if (type == 'c')
		len = ft_putchar(va_arg(args, int));
	else if (type == 's')
		len = ft_putstr(va_arg(args, const char *));
	else if (type == 'd' || type == 'i')
		len = ft_printnbr(va_arg(args, long));
	else if (type == 'x')
		len = ft_print_lowerhexa(va_arg(args, unsigned));
	else if (type == 'X')
		len = ft_print_upperhexa(va_arg(args, long));
	else if (type == 'u')
		len = ft_print_unsigned_dec(va_arg(args, unsigned int));
	else if (type == '%')
		len = ft_print_percent();
	else if (type == 'p')
		len = ft_printf_ptr(va_arg(args, size_t));
	return (len);
}

int	ft_printf(const char *format, ...)
{
	int		i;
	int		len;
	va_list	args;

	i = 0;
	len = 0;
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%')
		{
			len += ft_type(format[i + 1], args);
			i += 2;
		}
		else
		{
			ft_putchar(format[i]);
			i++;
			len++;
		}
	}
	va_end(args);
	return (len);
}

// int main (void)
// {
// 	int 	i;
// 	int  	j;
// 	int 	d = 125;	
// 	i = ft_printf("%p\n", &d);
// 	j = printf("%p\n", &d);
// 	printf ("ft = %d, v = %d\n",i, j);
// 	return (0);
// }

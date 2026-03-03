/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_check_args.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 14:42:56 by krfranco          #+#    #+#             */
/*   Updated: 2025/06/01 10:27:06 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	ft_strlen(char *c)
{
	size_t	count;

	count = 0;
	while (*c)
	{
		count++;
		c++;
	}
	return (count);
}

int	ft_isdigit(int c)
{
	if (c >= 48 && c <= 57)
		return (1);
	return (0);
}

int	check_args(char **av)
{
	int	i;
	int	y;
	int	rtn;

	i = 1;
	rtn = 0;
	while (av[i])
	{
		if (ft_strlen(av[i]) > 10 || ft_atoi(av[i]) < 0
			|| ft_atoi(av[i]) > INT_MAX)
			rtn = 1;
		y = 0;
		while (av[i][y])
		{
			if (!ft_isdigit(av[i][y]))
				rtn = 1;
			if (rtn == 1)
				return (ft_error("Invalid arguments\n"), 1);
			y++;
		}
		i++;
	}
	return (rtn);
}

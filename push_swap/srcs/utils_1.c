/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 02:43:39 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 01:07:30 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

long long	ft_atoi(char *str)
{
	int			i;
	long long	neg;
	long long	res;

	i = 0;
	res = 0;
	neg = 1;
	while ((str[i] <= 13 && str[i] >= 9) || str[i] == ' ')
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			neg *= -1;
		i++;
	}
	while (str[i] <= '9' && str[i] >= '0')
	{
		res = res * 10 + str[i] - '0';
		i++;
	}
	if (str[i] != '\0' && !(str[i] <= '9' && str[i] >= '0'))
		res = 0;
	return (res * neg);
}

int	is_valid(char **av, int i)
{
	int	j;

	while (av[i])
	{
		if ((*av[i] != '0' && ft_atoi(av[i]) == 0) || ft_strlen(av[i]) > 11
			|| ft_atoi(av[i]) > INT_MAX || ft_atoi(av[i]) < INT_MIN)
		{
			ft_printf("Error\n");
			return (0);
		}
		j = i + 1;
		while (av[j])
		{
			if (ft_atoi(av[i]) == ft_atoi(av[j]))
			{
				ft_printf("Error\n");
				return (0);
			}
			j++;
		}
		i++;
	}
	return (1);
}

void	ft_swap(int *a, int *b)
{
	int	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	printlink(t_linked *head)
{
	if (head)
	{
		while (head->next != NULL)
		{
			ft_printf("[%d]\n", head->data);
			head = head->next;
		}
		ft_printf("[%d]\n", head->data);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 19:22:19 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/12 22:48:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	need_space(char *av)
{
	int	i;

	i = 0;
	while (av[i])
	{
		if (av[i] != ' ')
			return (0);
		i++;
	}
	return (1);
}

int	main(int ac, char **av)
{
	t_linked	*stack_a;
	t_linked	*stack_b;

	if (ac > 1)
	{
		if (av[1][0] == '\0' || need_space(av[1]))
		{
			ft_printf("Error\n");
			return (-1);
		}
		stack_a = assign(ac, av);
		stack_b = NULL;
		if (stack_a)
		{
			pick_sort(&stack_a, &stack_b);
			free_stack(&stack_a);
		}
	}
	return (0);
}

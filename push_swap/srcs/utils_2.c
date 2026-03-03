/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 19:30:49 by krfranco          #+#    #+#             */
/*   Updated: 2024/03/07 02:54:19 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

t_linked	*assign(int ac, char **av)
{
	t_linked	*stack_a;

	stack_a = malloc(sizeof(t_linked));
	if (!stack_a)
		return (NULL);
	if (ac == 2)
		stack_a = is_valid_split(stack_a, av);
	else
	{
		if (is_valid(av, 1) && ac > 2)
			fill_link(stack_a, av, ac, 0);
		else
		{
			free(stack_a);
			stack_a = NULL;
		}
	}
	return (stack_a);
}

void	fill_link(t_linked *head, char **av, int ac, int flag)
{
	int	i;

	if (!flag)
	{
		initlink(head, ft_atoi(av[1]));
		i = 2;
	}
	else
	{
		initlink(head, ft_atoi(av[0]));
		i = 1;
	}
	while (i < ac)
	{
		addlink(head, ft_atoi(av[i]));
		head = head ->next;
		i++;
	}
}

t_linked	*is_valid_split(t_linked *stack_a, char **av)
{
	char		**tab;

	tab = ft_split(av[1], ' ');
	if (!tab)
		return (NULL);
	else
	{
		if (is_valid(tab, 0))
		{
			fill_link(stack_a, tab, (ft_count(av[1], ' ')), 1);
			free_split(tab);
			return (stack_a);
		}
		else
		{
			free_split(tab);
			free(stack_a);
			return (NULL);
		}
	}
}

void	initlink(t_linked *head, int val)
{
	head->data = val;
	head->next = NULL;
	head->prev = NULL;
}

void	addlink(t_linked *head, int val)
{
	head->next = malloc(sizeof(t_linked));
	head->next->prev = head;
	head->next->data = val;
	head->next->next = NULL;
}

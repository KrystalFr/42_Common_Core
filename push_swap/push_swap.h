/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/25 19:19:45 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 02:04:31 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "ft_Printf/ft_printf.h"
# include <limits.h>
# include <stdbool.h>

typedef struct s_list
{
	int				data;
	int				index;
	int				cost;
	bool			cheap;
	bool			med;
	struct s_list	*target;
	struct s_list	*next;
	struct s_list	*prev;
}	t_linked;

// ---Init and fill---

t_linked	*assign(int ac, char **av);
void		addlink(t_linked *head, int val);
void		initlink(t_linked *head, int val);
void		fill_link(t_linked *head, char **av, int ac, int flag);
int			is_valid(char **av, int i);
t_linked	*is_valid_split(t_linked *stack_a, char **av);

// ---Init and fill utils---

void		printlink(t_linked *head);
void		free_stack(t_linked **stack);
void		free_split(char **tab);
void		ft_swap(int *a, int *b);
char		**ft_split(char const *s, char c);
char		*ft_strndup(char *src, int len);
int			ft_count(char const *str, char c);
int			ft_strlen(char *str);
long long	ft_atoi(char *str);

// ---Movements---

void		pa_pb(t_linked **give, t_linked **take, char c);
void		sa_sb(t_linked **head, char c);
void		ss(t_linked **stack_a, t_linked **stack_b);
void		ra_rb(t_linked **stack, char c);
void		rr(t_linked **stack_a, t_linked **stack_b);
void		rra_rrb(t_linked **stack, char c);
void		rrr(t_linked **stack_a, t_linked **stack_b);

// ---Movements utils---

void		givetoplink(t_linked **give, t_linked **take);
void		moveup(t_linked **stack);
void		movedown(t_linked **stack);
void		rr_2(t_linked **stack_a, t_linked **stack_b, t_linked *tmp);
void		rrr_2(t_linked **stack_a, t_linked **stack_b, t_linked *tmp);
void		rotate_solo(t_linked **stack, t_linked *tmp, char c);

// ---Algo small---

void		diet_algo(t_linked **stack_a, t_linked **stack_b);
void		three(t_linked **stack_a);
void		put_smallest_on_top(t_linked **stack_a);

// ---Algo big---

void		algo_vegan(t_linked **stack_a, t_linked **stack_b);
void		step_one(t_linked **stack_a, t_linked **stack_b);
void		step_two(t_linked **stack_a, t_linked **stack_b);
void		move_a_to_b(t_linked **stack_a, t_linked **stack_b);
void		set_index(t_linked **stack);
void		set_cheapest(t_linked **stack);
void		set_target_a(t_linked **stack_a, t_linked **stack_b);
void		get_cost_a(t_linked **stack_a, t_linked **stack_b);
void		set_target_b(t_linked **stack_a, t_linked **stack_b);

// ---Algo utils---

void		pick_sort(t_linked **stack_a, t_linked **stack_b);
void		set_index(t_linked **stack);
int			stack_len(t_linked *stack);
int			is_sorted(t_linked	*stack);
int			find_smallest(t_linked **stack);
int			find_biggest(t_linked **stack);
int			one_before(t_linked **stack, int bef);
int			last_data(t_linked **stack);
int			get_len(t_linked **stack_a, int i);
t_linked	*get_cheapest(t_linked **stack);
t_linked	*get_biggest(t_linked *stack);
t_linked	*get_smallest(t_linked *stack);
#endif
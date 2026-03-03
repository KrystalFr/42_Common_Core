/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 20:23:47 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 15:41:11 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
// cas 1: si pas d'option ou argument, exit le bon nombre
// cas 2: si un argument:
// -1 regarder si deuxieme argument existe
//     -> si oui, print "exit\nMichell: exit: too many arguments"
// -2 regarder si premier argument est un nombre
//     -> si non,
	exit avec 2 et print "exit\nMichell: exit%s : numeric argument required
//     -> si oui, le stocker % 256
// 			->si second argument existe:
				print "exit\nMichell: exit: too many arguments"
*/

// le vrai exit gere jusqu'au long max et long min en arguments
// ici on ne gere que les int max et int mins, prcke flm

long	ft_atol(const char *nptr)
{
	int		neg;
	int		i;
	long	num;

	i = 0;
	neg = 1;
	num = 0;
	while (nptr[i] == ' ' || nptr[i] == '\n' || nptr[i] == '\t'
		|| nptr[i] == '\v' || nptr[i] == '\f' || nptr[i] == '\r')
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			neg *= -1;
		i++;
	}
	while (nptr[i] >= 48 && nptr[i] <= 57)
	{
		num = num * 10 + (nptr[i] - 48);
		i++;
	}
	return (num * neg);
}

bool	is_number(char *str)
{
	size_t	i;
	long	j;

	i = 0;
	i = get_token_start_index(str, 0);
	if (i == ft_strlen(str))
		return (false);
	j = 0;
	while (str[i + j] == '+' || str[i + j] == '-')
		j++;
	if (j > 1)
		return (false);
	i += j;
	j = 0;
	while (str[i + j] && ft_isdigit(str[i + j]))
		j++;
	if (j > 11 || j == 0)
		return (false);
	i += j;
	j = ft_atol(str);
	if (j > LONG_MAX || j < LONG_MIN)
		return (false);
	if (get_token_start_index(str, i) != (int)ft_strlen(str))
		return (false);
	return (true);
}

void	handle_exit_too_many_args(t_minishell *vars)
{
	vars->exit_value = 2;
	ft_putstr_fd("exit\nMichell: exit: too many arguments\n", 2);
	vars->should_exit_minishell = 0;
	if (!command_should_be_execute_in_the_parent(*vars->head))
		exit_minishell(vars, NULL);
}

void	handle_no_numeric_argument(t_minishell *vars, char *str)
{
	vars->exit_value = 2;
	printf("exit\nMichell: exit: %s : numeric argument required\n", str);
	if (!command_should_be_execute_in_the_parent(*vars->head))
		exit_minishell(vars, NULL);
}

void	exec_exit(t_minishell *vars, t_token *token)
{
	char	**tab;

	if (!command_should_be_execute_in_the_parent(token))
		close_both_pipe(vars, token);
	if (token->redirection && (token->redirection->in_fd == -1
			|| token->redirection->out_fd == -1))
		return ;
	vars->should_exit_minishell = 1;
	if (!token->executable_tokens[1])
		return (exit_minishell(vars, NULL));
	tab = token->executable_tokens;
	if (is_number(tab[1]) && tab[2])
		return (close_both_pipe(vars, token), handle_exit_too_many_args(vars));
	else if (!is_number(tab[1]))
		return (close_both_pipe(vars, token), handle_no_numeric_argument(vars,
				tab[1]));
	close_both_pipe(vars, token);
	vars->exit_value = ft_atoi(tab[1]);
	exit_minishell(vars, NULL);
}

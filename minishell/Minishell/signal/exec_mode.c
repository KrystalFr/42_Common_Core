/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_mode.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/15 06:56:36 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:11:25 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_sigquit_exec_mode(int sig)
{
	(void)sig;
	g_signal_state = SIGQUIT;
	printf("Quit (core dumped)\n");
}

void	handle_sigint_exec_mode(int sig)
{
	t_minishell		*vars;

	(void)sig;
	g_signal_state = SIGINT;
	printf("\n");
	vars = get_vars(NULL);
	vars->exit_value = 130;
	rl_on_new_line();
	rl_replace_line("", 0);
}

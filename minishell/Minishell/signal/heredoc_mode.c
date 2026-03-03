/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_mode.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:25:49 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:11:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	handle_sigint_heredoc_mode(int sig)
{
	t_minishell	*vars;

	(void)sig;
	g_signal_state = SIGINT;
	printf("\n");
	close(0);
	vars = get_vars(NULL);
	vars->exit_value = 130;
	rl_on_new_line();
	rl_replace_line("", 0);
}

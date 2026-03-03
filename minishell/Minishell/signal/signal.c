/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 05:30:32 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:10:19 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*

ctrl c Interrompt la commande en cours,
	affiche un retour à la ligne (\n) mais ne quitte pas le shell

ctrl + \ Ignoré dans Bash interactif (mais dans un pipeline,
	il affiche Quit (core dumped))

ctrl + D Ferme le shell

*/

volatile sig_atomic_t	g_signal_state = 0;

void	signal_handler(int flag)
{
	if (flag == HEREDOC_MODE)
	{
		signal(SIGINT, handle_sigint_heredoc_mode);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (flag == PROMPT_MODE)
	{
		signal(SIGINT, handle_sigint_prompt_mode);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (flag == EXEC_MODE)
	{
		signal(SIGINT, handle_sigint_exec_mode);
		signal(SIGQUIT, handle_sigquit_exec_mode);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 05:31:26 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 19:20:58 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Le lexer tokenise l'input
// le parser determine si oui ou non l'input est valide
// l'expander remplace les variables par leur valeur
// l'exec execute les commandes

void	minishell(t_minishell *vars)
{
	lexer(vars);
	parser(vars);
	expander(vars);
	exec(vars);
}

int	main(int ac, char **av, char **env)
{
	t_minishell	vars;

	init_shell(ac, av, env, &vars);
	while (1)
	{
		signal_handler(PROMPT_MODE);
		vars.input = readline("Michell: ");
		if (vars.input == NULL)
		{
			vars.should_exit_minishell = 1;
			free_env(&vars);
			exit_minishell(&vars, "exiting Michel\n");
		}
		if (vars.input)
		{
			add_history(vars.input);
			minishell(&vars);
			free(vars.input);
			exit_minishell(&vars, NULL);
		}
	}
	return (0);
}

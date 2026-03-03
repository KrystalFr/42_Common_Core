/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 05:40:51 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 14:19:59 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	echo_option_is_valid(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (false);
	if (str[i] && str[i] != '-')
		return (false);
	i++;
	while (str[i])
	{
		if (str[i] != 'n')
			return (false);
		i++;
	}
	if (i == 1)
		return (false);
	return (true);
}

int	parse_echo_option(t_token *token, int i, int *is_option)
{
	while (token->executable_tokens[i]
		&& echo_option_is_valid(token->executable_tokens[i]))
	{
		*is_option = 1;
		i++;
	}
	return (i);
}

void	exec_echo(t_minishell *vars, t_token *token)
{
	int	is_option;
	int	i;

	i = 1;
	is_option = 0;
	i = parse_echo_option(token, i, &is_option);
	if (!dup_redirection_out(vars, token))
		return (close_both_pipe(vars, token), exit_minishell(vars, NULL));
	if (token->redirection && (token->redirection->in_fd == -1))
		return (close_both_pipe(vars, token), exit_minishell(vars, NULL));
	while (token->executable_tokens[i])
	{
		write(1, token->executable_tokens[i],
			ft_strlen(token->executable_tokens[i]));
		if (token->executable_tokens[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (!is_option)
		write(1, "\n", 1);
	close_both_pipe(vars, token);
	vars->exit_value = 0;
	exit_minishell(vars, NULL);
}

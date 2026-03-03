/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 20:32:36 by krfranco          #+#    #+#             */
/*   Updated: 2025/02/21 21:55:30 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	expand_delimiter(t_token *temp, t_minishell *vars)
{
	int	i;

	i = 0;
	temp->exitval = ft_itoa(vars->exit_value);
	i = clean_token(temp, vars);
	free(temp->exitval);
	if (i < 0)
		failed_malloc(vars);
	temp->token = temp->new_token;
}

void	failed_malloc(t_minishell *vars)
{
	printf("Error ; failed malloc\n");
	exit_minishell(vars, NULL);
}

int	is_maj(char c)
{
	if (c >= 'A' && c <= 'Z')
		return (1);
	return (0);
}

int	is_whitespace(char c)
{
	if ((c >= 9 && c <= 13) || c == 32
		|| c == '\0')
		return (1);
	return (0);
}

int	is_whitedollar(char c)
{
	if ((c >= 9 && c <= 13) || c == 32
		|| c == '\0' || c == '$' || c == '\"')
		return (1);
	return (0);
}

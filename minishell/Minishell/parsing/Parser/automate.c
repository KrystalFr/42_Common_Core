/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   automate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 16:26:01 by krfranco          #+#    #+#             */
/*   Updated: 2025/02/21 22:40:38 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	check_token(t_minishell *vars)
{
	t_token	*temp;

	temp = *vars->head;
	while (temp)
	{
		temp->token_type = get_token_type(temp->token[0]);
		temp = temp->next;
	}
	temp = *vars->head;
	while (temp)
	{
		if (parse_error(temp, vars))
			break ;
		temp = temp->next;
	}
}

int	parse_error(t_token *temp, t_minishell *vars)
{
	if (temp->token_type == PIPE)
	{
		if (is_pipe(temp, vars))
			return (1);
	}
	else if (temp->token_type == REDIRECTION)
	{
		if (is_redirection(temp, vars))
			return (1);
	}
	else if ((temp->token_type == WORD || temp->token_type == DQUOTE
			|| temp->token_type == QUOTE))
	{
		if (is_word(temp, vars))
			return (1);
	}
	else
	{
		printf("Michell: syntax error near unexpected token \'%s\'\n",
			temp->token);
		exit_minishell(vars, NULL);
		return (1);
	}
	return (0);
}

int	is_pipe(t_token *temp, t_minishell *vars)
{
	if (!temp->prev || !temp->next || temp->next->token_type == PIPE)
	{
		printf("Michell: syntax error near unexpected token \'%s\'\n",
			temp->token);
		exit_minishell(vars, NULL);
		return (1);
	}
	return (0);
}

int	is_redirection(t_token *temp, t_minishell *vars)
{
	if (!temp->next || (temp->next->token_type != WORD
			&& temp->next->token_type != QUOTE
			&& temp->next->token_type != DQUOTE))
	{
		printf("Michell: syntax error near unexpected token \'%s\'\n",
			temp->token);
		exit_minishell(vars, NULL);
		return (1);
	}
	if (!ft_strncmp(temp->token, "<<", 2))
	{
		if (!get_here_doc(vars, temp))
			return (1);
	}
	return (0);
}

int	is_word(t_token *temp, t_minishell *vars)
{
	if (invalid_word(temp))
	{
		if (invalid_word(temp) == 1)
			printf("Michell: syntax error near unexpected\
				token \'%s\'\n", temp->token);
		if (invalid_word(temp) == 2)
			printf("Michell: syntax error near unexpected\
				token \'%.2s\'\n", temp->token);
		exit_minishell(vars, NULL);
		return (1);
	}
	if (invalid_quotes(temp, vars))
	{
		exit_minishell(vars, NULL);
		return (1);
	}
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   automate_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 11:17:07 by krfranco          #+#    #+#             */
/*   Updated: 2025/02/21 21:17:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	write_tokentype(int type)
{
	if (type == 0 || type == 2 || type == 3)
		printf("Word\n");
	if (type == 5)
		printf("Pipe\n");
	if (type == 1)
		printf("Redirection\n");
	if (type == 4)
		printf("Whitespace\n");
	return ;
}

int	invalid_word(t_token *temp)
{
	char	list[3];
	int		i;

	list[0] = '&';
	list[1] = '\\';
	list[2] = '\0';
	i = 0;
	while (list[i])
	{
		if (list[i] == temp->token[0] && list[i] == temp->token[1])
			return (2);
		else if (list[i] == temp->token[0])
			return (1);
		i++;
	}
	return (0);
}

int	invalid_quotes(t_token *temp, t_minishell *vars)
{
	int	i;

	i = 0;
	while (temp->token[i])
	{
		if (temp->token[i] == '\"')
		{
			i = double_quote(i, temp, vars);
			if (i < 0)
				break ;
		}
		else if (temp->token[i] == '\'')
		{
			i = single_quote(i, temp, vars);
			if (i < 0)
				break ;
		}
		i++;
	}
	if (i < 0)
		return (1);
	return (0);
}

int	double_quote(int i, t_token *temp, t_minishell *vars)
{
	int	count;

	count = 0;
	while (temp->token[i])
	{
		if (temp->token[i] == '\"')
			count++;
		if (count == 2)
			break ;
		i++;
	}
	if (count % 2 != 0)
	{
		printf("Michell: syntax error open quotes <\">\n");
		exit_minishell(vars, NULL);
		return (-1);
	}
	return (i);
}

int	single_quote(int i, t_token *temp, t_minishell *vars)
{
	int	count;

	count = 0;
	while (temp->token[i])
	{
		if (temp->token[i] == '\'')
			count++;
		if (count == 2)
			break ;
		i++;
	}
	if (count % 2 != 0)
	{
		printf("Michell: syntax error open quotes <\'>\n");
		exit_minishell(vars, NULL);
		return (-1);
	}
	return (i);
}

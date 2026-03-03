/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 11:08:54 by krfranco          #+#    #+#             */
/*   Updated: 2025/02/21 22:12:50 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	expander(t_minishell *vars)
{
	if (vars->head == NULL)
		return ;
	expand(vars);
}

void	expand(t_minishell *vars)
{
	t_token	*temp;
	int		i;

	i = 0;
	temp = *vars->head;
	while (temp)
	{
		if (ft_strcmp(temp->token, "$") && (temp->token_type == WORD
				|| temp->token_type == QUOTE || temp->token_type == DQUOTE))
		{
			temp->exitval = ft_itoa(vars->exit_value);
			i = clean_token(temp, vars);
			free(temp->exitval);
			if (i < 0)
				break ;
			else
				temp->token = temp->new_token;
		}
		temp = temp->next;
	}
	if (i < 0)
		failed_malloc(vars);
}

int	clean_token(t_token *temp, t_minishell *vars)
{
	temp->fi = 0;
	temp->in_squote = 0;
	temp->in_dquote = 0;
	if (count_expand(temp, vars) < 0)
		return (-1);
	temp->new_token = ft_malloc(temp->final_size + 1);
	if (temp->new_token == NULL)
		return (-1);
	temp->in_squote = 0;
	temp->in_dquote = 0;
	if (is_expandable(temp, vars) < 0)
		return (-1);
	temp->new_token[temp->fi] = '\0';
	return (0);
}

int	is_expandable(t_token *temp, t_minishell *vars)
{
	int	i;

	i = 0;
	while (temp->token[i])
	{
		if (temp->token[i] == '\"')
			i = expandable_dquote(temp, i);
		else if (temp->token[i] == '\'')
			i = expandable_squote(temp, i);
		else if (temp->token[i] == '$' && !temp->in_squote
			&& temp->token[i + 1] == '?')
			i = expand_exitval(temp, i);
		else if (temp->token[i] == '$' && !temp->in_squote
			&& !is_whitedollar(temp->token[i + 1]))
			i = expandable_env(temp, i, vars);
		else
		{
			temp->new_token[temp->fi] = temp->token[i];
			temp->fi++;
			i++;
		}
		if (i < 0)
			return (-1);
	}
	return (i);
}

int	count_expand(t_token *temp, t_minishell *vars)
{
	int	i;

	i = 0;
	temp->final_size = 0;
	while (temp->token[i])
	{
		if (temp->token[i] == '\"')
			i = count_dquote(temp, i);
		else if (temp->token[i] == '\'')
			i = count_squote(temp, i);
		else if (temp->token[i] == '$' && !temp->in_squote
			&& temp->token[i + 1] == '?')
			i = count_exitval(temp, i);
		else if (temp->token[i] == '$' && !temp->in_squote
			&& !is_whitedollar(temp->token[i + 1]))
			i = count_env(temp, i, vars);
		else
		{
			temp->final_size++;
			i++;
		}
		if (i < 0)
			break ;
	}
	return (i);
}

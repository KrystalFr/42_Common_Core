/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/16 05:27:54 by krfranco          #+#    #+#             */
/*   Updated: 2025/02/21 21:56:05 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	count_squote(t_token *temp, int i)
{
	if (!temp->in_dquote)
		temp->in_squote = !temp->in_squote;
	if (temp->in_dquote)
		temp->final_size++;
	i++;
	return (i);
}

int	count_dquote(t_token *temp, int i)
{
	if (!temp->in_squote)
		temp->in_dquote = !temp->in_dquote;
	if (temp->in_squote)
		temp->final_size++;
	i++;
	return (i);
}

int	count_exitval(t_token *temp, int i)
{
	temp->final_size += ft_strlen(temp->exitval);
	i += 2;
	return (i);
}

int	count_env(t_token *temp, int i, t_minishell *vars)
{
	int		j;
	char	*to_replace;

	j = i + 1;
	if (temp->token[i] == '$' && is_whitedollar(temp->token[i + 1]))
		return (i - 1);
	while (!is_whitedollar(temp->token[j]) && temp->token[j] != '\"')
		j++;
	to_replace = ft_malloc((j + 1) - i);
	if (!to_replace)
		return (-1);
	j = 0;
	i++;
	while (!is_whitedollar(temp->token[i]) && temp->token[i] != '\"')
	{
		to_replace[j] = temp->token[i];
		j++;
		i++;
	}
	to_replace[j] = '\0';
	env_size(vars, to_replace, temp);
	return (i);
}

void	env_size(t_minishell *vars, char *to_replace, t_token *temp)
{
	t_env	*tmpenv;

	tmpenv = vars->env[0];
	while (tmpenv)
	{
		if ((ft_strncmp(tmpenv->data, to_replace, ft_strlen(to_replace)) == 0)
			&& (tmpenv->data[ft_strlen(to_replace)] == '='))
			break ;
		else
			tmpenv = tmpenv->next;
	}
	if (tmpenv == NULL)
		return ;
	temp->final_size -= (ft_strlen(to_replace) + 1);
	temp->final_size += ft_strlen(tmpenv->data);
}

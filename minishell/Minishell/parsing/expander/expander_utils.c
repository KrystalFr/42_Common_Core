/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 11:08:52 by krfranco          #+#    #+#             */
/*   Updated: 2025/02/21 22:10:19 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	expandable_squote(t_token *temp, int i)
{
	if (!temp->in_dquote)
		temp->in_squote = !temp->in_squote;
	if (temp->in_dquote)
	{
		temp->new_token[temp->fi] = temp->token[i];
		temp->fi++;
	}
	i++;
	return (i);
}

int	expandable_dquote(t_token *temp, int i)
{
	if (!temp->in_squote)
		temp->in_dquote = !temp->in_dquote;
	if (temp->in_squote)
	{
		temp->new_token[temp->fi] = temp->token[i];
		temp->fi++;
	}
	i++;
	return (i);
}

int	expand_exitval(t_token *temp, int i)
{
	int	j;

	j = 0;
	while (temp->exitval[j])
	{
		temp->new_token[temp->fi] = temp->exitval[j];
		j++;
		temp->fi++;
	}
	i += 2;
	return (i);
}

int	expandable_env(t_token *temp, int i, t_minishell *vars)
{
	int		j;
	int		k;
	char	*to_replace;

	j = i + 1;
	if (temp->token[i] == '$' && is_whitedollar(temp->token[i + 1]))
		return (i - 1);
	while (!is_whitedollar(temp->token[j]) && temp->token[j] != '\"')
		j++;
	to_replace = ft_malloc((j) - i);
	if (!to_replace)
		return (-1);
	j = 0;
	i++;
	k = i;
	while (!is_whitedollar(temp->token[k]) && temp->token[k] != '\"')
	{
		to_replace[j] = temp->token[k];
		j++;
		k++;
	}
	to_replace[j] = '\0';
	i = write_env(vars, to_replace, i, temp);
	return (i);
}

int	write_env(t_minishell *vars, char *to_replace, int i, t_token *temp)
{
	int		j;
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
		return (i + ft_strlen(to_replace));
	j = ft_strlen(to_replace) + 1;
	while (tmpenv->data[j])
	{
		temp->new_token[temp->fi] = tmpenv->data[j];
		j++;
		temp->fi++;
	}
	return (i + (ft_strlen(to_replace)));
}

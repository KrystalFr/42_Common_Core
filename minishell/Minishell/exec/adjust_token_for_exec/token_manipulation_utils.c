/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_manipulation_utils.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 00:42:44 by legoat            #+#    #+#             */
/*   Updated: 2025/02/15 10:15:13 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// soit une liste chainée de tokens:
// a->b->c->d et c le token que l'on veut remplacer par z
// replace token modifie la liste chainée pour qu'elle devienne:
// a->b->z->d
// elle est moche cette fonction, mais elle marche.

void	replace_token(t_minishell *vars, t_token *old_token, t_token *new_token)
{
	t_token	*new_prev;
	t_token	*new_next;

	new_prev = old_token->prev;
	new_next = old_token->next;
	if (new_prev)
		new_token->prev = new_prev;
	if (new_next)
		new_token->next = new_next;
	if (new_prev)
		new_prev->next = new_token;
	if (new_next)
		new_next->prev = new_token;
	if (*vars->head == old_token)
		*vars->head = new_token;
}

// Bon heu flm d expliquer la fonction fait ce qu'elle dit.
// Exemple a->b->c->d et on veut supprimer c.
// La liste devient a->b->d

void	delete_token(t_minishell *vars, t_token *token)
{
	t_token	*prev;
	t_token	*next;

	prev = token->prev;
	next = token->next;
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
	if (*vars->head == token)
	{
		*vars->head = next;
		if (*vars->head)
			(*vars->head)->prev = NULL;
	}
}

// cette fonction insere token_insert apres token
// a->b->c et on veut inserer z apres b
// la liste devient a->b->z->c

void	insert_token(t_token *token, t_token *token_insert)
{
	token_insert->prev = token;
	token_insert->next = token->next;
	token->next = token_insert;
	if (token_insert->next)
		token_insert->next->prev = token_insert;
}

// revoie le type exact du token redirection

int	get_redirection_token_type(t_token *token)
{
	if (ft_strncmp(token->token, "<", ft_strlen(token->token)) == 0)
		return (IN_REDIR);
	else if (ft_strncmp(token->token, ">", ft_strlen(token->token)) == 0)
		return (OUT_REDIR);
	else if (ft_strncmp(token->token, ">>", ft_strlen(token->token)) == 0)
		return (OUT_APPEND_REDIR);
	else if (token->token_type == QUOTE || token->token_type == DQUOTE)
		return (WORD);
	return (token->token_type);
}

// renvoie la taille de la liste chainée de tokens

int	token_list_size(t_token *head)
{
	t_token		*token;
	int			size;

	size = 0;
	token = head;
	while (token)
	{
		size++;
		token = token->next;
	}
	return (size);
}

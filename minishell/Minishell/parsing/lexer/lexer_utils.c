/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 19:13:37 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/11 02:25:37 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// renvoie l'index + 1 du deuxieme
// token ' s il y en a un

int	handle_quotes(char *input, int end)
{
	end++;
	while (input[end] && input[end] != '\'')
		end++;
	if (input[end] == '\'')
		end++;
	return (end);
}

// renvoie l'index + 1 du deuxieme
// token " s il y en a un

int	handle_dquotes(char *input, int end)
{
	end++;
	while (input[end] && input[end] != '"')
		end++;
	if (input[end] == '"')
		end++;
	return (end);
}

// gere les mots, et surtout le cas :
// asd"asd"asd qui doit etre considere
// comme un seul token

int	handle_words(char *input, int start)
{
	int	token_type;

	while (input[start])
	{
		token_type = get_token_type(input[start]);
		if (token_type == QUOTE)
			start = handle_quotes(input, start);
		else if (token_type == DQUOTE)
			start = handle_dquotes(input, start);
		else if (token_type == WORD)
			start++;
		else
			break ;
	}
	return (start);
}

t_token	*create_token(t_minishell *vars, char *str, int token_size)
{
	t_token	*new_node;

	new_node = ft_malloc(sizeof(t_token));
	new_node->token = ft_malloc(sizeof(char) * (token_size + 1));
	if (!new_node || !new_node->token)
		exit_minishell(vars, "malloc error\n");
	ft_strlcpy(new_node->token, str, token_size + 1);
	new_node->token_type = get_token_type(new_node->token[0]);
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

// ajoute un node a la liste chainee
// et met la valeur token dans la variable
// mode->token.
// La liste chainee est constituee de :
// t_token -> next.

void	add_token_to_linked_list(t_lexer *d)
{
	t_token	*temp;
	int		token_size;

	token_size = d->end - d->start;
	if (token_size == 0)
		return ;
	d->new_node = create_token(d->vars, d->input + d->start, token_size);
	if (!d->new_node)
	{
		d->head = NULL;
		d->start = ft_strlen(d->input);
		return ;
	}
	if (*d->head == NULL)
		*d->head = d->new_node;
	else
	{
		temp = *d->head;
		while (temp->next)
			temp = temp->next;
		temp->next = d->new_node;
		d->new_node->prev = temp;
	}
}

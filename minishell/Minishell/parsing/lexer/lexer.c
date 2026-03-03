/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 18:34:34 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 19:36:52 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// renvoie le type de token selon le caractere 'c'

int	get_token_type(char c)
{
	if (c == '|')
		return (PIPE);
	if (c == '<' || c == '>')
		return (REDIRECTION);
	if ((c >= 9 && c <= 13) || c == 32)
		return (WHITESPACE);
	if (c == '\'')
		return (QUOTE);
	if (c == '\"')
		return (DQUOTE);
	return (WORD);
}

// skip les whitespace et place renvoie l'index
// sur le debut du token

int	get_token_start_index(char *input, int end)
{
	int	start;

	start = end;
	while (input[start] && get_token_type(input[start]) == WHITESPACE)
		start++;
	return (start);
}

// determine l'index de fin du token selon son type

int	get_token_end_index(char *input, int start)
{
	int	token_type;
	int	end;

	end = start;
	token_type = get_token_type(input[start]);
	if (token_type == QUOTE)
		end = handle_words(input, start);
	else if (token_type == DQUOTE)
		end = handle_words(input, start);
	else if (token_type == WORD)
		end = handle_words(input, start);
	else if (token_type == REDIRECTION)
	{
		if (input[end + 1] && input[end] == input[end + 1])
			end++;
		end++;
	}
	else if (token_type == PIPE)
		end++;
	return (end);
}

// get_tokens parcourt l'input, place les variables start et end
// a l'index de debut et de fin de chaques token.
// Ensuite, chaques tokens est stocke dans une structure t_token.
// chaques maillons est ajoute dans une liste chainee.
// Quand la liste est finie, la fonction get_command_table
// alloue un tableau de node de t_token de taille lst_size
// et le remplis de token.

void	get_tokens(t_lexer *d)
{
	while (1)
	{
		d->start = get_token_start_index(d->input, d->end);
		d->end = get_token_end_index(d->input, d->start);
		add_token_to_linked_list(d);
		if (d->end >= d->input_size || d->start >= d->input_size)
			break ;
	}
}

// Le lexer sert a transformer la chaine de caractere en tokens.
// cette fonction initialise toutes les variables dont le lexer
// a besoin et les mets dans la structure t_lexer. Elle ajoute
// la tete de la liste chainee dans la structure t_minishell.

void	lexer(t_minishell *vars)
{
	t_lexer	data;

	g_signal_state = 0;
	data.vars = vars;
	data.start = 0;
	data.end = 0;
	data.input = vars->input;
	data.input_size = ft_strlen(vars->input);
	data.head = ft_malloc(sizeof(t_token *));
	if (!data.head)
		return (exit_minishell(vars, "malloc error\n"));
	*data.head = NULL;
	get_tokens(&data);
	vars->head = data.head;
	if (*vars->head == NULL)
		return (exit_minishell(vars, NULL));
}

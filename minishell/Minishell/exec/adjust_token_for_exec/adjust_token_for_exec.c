/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   adjust_token_for_exec.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 09:43:56 by legoat            #+#    #+#             */
/*   Updated: 2025/02/21 22:30:57 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// cette fonction transforme << delimiter | cat
// en: nom_du_here_doc | cat
// et assigne le type HERE_DOC au nouveau token cree.

void	adjust_heredoc(t_minishell *vars)
{
	t_token		*new_token;
	t_token		*token;
	t_heredoc	*heredoc;

	token = *vars->head;
	heredoc = vars->heredoc_list;
	while (token)
	{
		if (!ft_strncmp(token->token, "<<", 2))
		{
			new_token = create_token(vars, heredoc->name,
					ft_strlen(heredoc->name));
			replace_token(vars, token, new_token);
			delete_token(vars, token->next);
			token = new_token;
			token->token_type = HERE_DOC;
			heredoc = heredoc->next;
		}
		token = token->next;
	}
}

// cette fonction determine la commande de chaque segment, si il y en a une.

void	determine_command(t_minishell *vars, t_segment *segment)
{
	t_token	*token;

	token = segment->start;
	while (1)
	{
		if (token->token_type == WORD)
		{
			token->token_type = COMMAND;
			token->executable_tokens = create_str_tab(vars, token->token);
			token->redirection = segment->redirection;
			return ;
		}
		if (token == segment->end)
			break ;
		token = token->next;
	}
	segment->stop_analizing = 1;
	token = create_token(vars, "notoken", 7);
	token->token_type = NO_COMMAND;
	token->redirection = NULL;
	insert_token(segment->start, token);
}

// cette fonction determine les options et les arguments de chaque segment.

void	determine_options_and_arguments(t_minishell *vars, t_segment **s)
{
	t_token	*command_token;
	t_token	*token;

	command_token = get_command_token(*s);
	if (!command_token)
		return ;
	token = command_token;
	while (1)
	{
		if (token->token_type == QUOTE || token->token_type == DQUOTE)
			token->token_type = WORD;
		if (token->token_type == WORD)
		{
			if (token->token[0] == '-')
				token->token_type = OPTION;
			else
				token->token_type = ARGUMENT;
			command_token->executable_tokens = add_str_to_tab(vars,
					command_token->executable_tokens, token->token);
			token->token_type = OPTION;
		}
		if (token == (*s)->end)
			break ;
		token = token->next;
	}
}

// delete tout les tokens les commandes

void	delete_useless_tokens(t_minishell *vars)
{
	t_token	*token;

	token = *vars->head;
	while (token)
	{
		if (token->token_type != COMMAND && token->token_type != NO_COMMAND)
			delete_token(vars, token);
		token = token->next;
	}
	token = *vars->head;
	while (token && (token->token_type == NO_COMMAND
			|| token->token_type == COMMAND))
		token = token->next;
	if (token)
		token->prev->next = NULL;
}

// Pour determiner le type exact de chaque tokens, BASH suit cette procedure:
// D'abord diviser la commande par pipe et traiter chaque segment separement.
// Pour chaque segment:
// 1/ determiner les redirections et les fichiers associes.
// 2/ Le token de type WORD le plus a gauche prends le type COMMAND.
// 3/ Assigner aux tokens commencant par - le type OPTION.
// 4/ Assigner aux derniers tokens de type WORD le type ARGUMENT.

// cat < test0 | cat < Makefile -e > test1 < test2 > test3 | cat > test4

// cat < test0 | cat < Makefile -e > test1 < test2 > test3 | cat > test4

// ls -la > out | cat > out < DAFDSF

void	adjust_token_for_exec(t_minishell *vars)
{
	t_segment	*segment;

	create_segment_struct(vars, &segment);
	adjust_heredoc(vars);
	while (1)
	{
		determine_segment(vars, &segment);
		if (!segment)
			break ;
		determine_redirection_and_file(vars, &segment);
		determine_command(vars, segment);
		determine_options_and_arguments(vars, &segment);
	}
	delete_useless_tokens(vars);
	if (!*vars->head)
		exit_minishell(vars, NULL);
}

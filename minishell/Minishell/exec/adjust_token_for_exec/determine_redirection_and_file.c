/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   determine_redirection_and_file.c                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 03:59:26 by legoat            #+#    #+#             */
/*   Updated: 2025/02/21 22:30:28 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// dans un premier temps, verifier si le fichier existe,
// puis verifier les autorisations de lecture
// puis ouvrir le fichier en lecture seule et stocker le fd
// dans redirection->in_fd.

t_heredoc	*get_right_heredoc(t_minishell *vars, t_token *token)
{
	t_heredoc	*hd;

	hd = vars->heredoc_list;
	while (1)
	{
		if (!ft_strncmp(hd->name, token->token, ft_strlen(token->token)))
			break ;
		hd = hd->next;
	}
	return (hd);
}

void	handle_in_redirection(t_minishell *vars, t_token *token,
		t_redirection *redirection, t_segment **segment)
{
	if (token->token_type == IN_REDIR)
		token->next->token_type = FILE;
	if ((*segment)->stop_analizing)
		return ;
	if (token->token_type == HERE_DOC)
		redirection->in_file = get_right_heredoc(vars, token)->name;
	else
		redirection->in_file = token->next->token;
	open_infile(redirection);
	if (redirection->in_fd == -1)
	{
		vars->exit_value = 1;
		(*segment)->stop_analizing = 1;
		if (access(token->next->token, F_OK) == -1)
			ft_printf("Michell: %s: No such file or directory\n",
				token->next->token);
		else
		{
			ft_printf("Michell: %s: Permission denied\n", token->next->token);
			vars->exit_value = 126;
		}
		return ;
	}
	close_file(redirection->in_fd);
}

void	handle_out_redirection(t_minishell *vars, t_token *token,
	t_redirection *redirection, t_segment **segment)
{
	token->next->token_type = FILE;
	if ((*segment)->stop_analizing)
		return ;
	redirection->out_file = token->next->token;
	redirection->redirection_type = token->token_type;
	open_outfile(redirection);
	if (redirection->out_fd == -1)
	{
		(*segment)->stop_analizing = 1;
		ft_printf("Michell: %s: Permission denied\n", token->next->token);
		vars->exit_value = 126;
		return ;
	}
	close_file(redirection->out_fd);
}

void	determine_redirection_and_file(t_minishell *vars, t_segment **s)
{
	t_token			*token;
	t_redirection	*redirection;

	token = (*s)->start;
	init_redirection(&redirection);
	while (1)
	{
		if (token && (token->token_type == REDIRECTION
				|| token->token_type == HERE_DOC))
		{
			token->token_type = get_redirection_token_type(token);
			if (token->token_type == IN_REDIR || token->token_type == HERE_DOC)
				handle_in_redirection(vars, token, redirection, s);
			else if (token->token_type == OUT_REDIR
				|| token->token_type == OUT_APPEND_REDIR)
				handle_out_redirection(vars, token, redirection, s);
			if (token->token_type == HERE_DOC)
				token = token->next;
		}
		if (!token || token == (*s)->end)
			break ;
		token = token->next;
	}
	(*s)->redirection = redirection;
	add_redirection_to_linked_list(vars, redirection);
}

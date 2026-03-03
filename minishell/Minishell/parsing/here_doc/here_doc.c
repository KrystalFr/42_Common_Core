/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 15:53:12 by legoat            #+#    #+#             */
/*   Updated: 2025/02/21 22:49:01 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// cette fonction est appelee par store_here_doc pour stocker un here_doc
// dans la liste de here_doc de la structure t_minishell. Elle parcourt
// la liste de here_doc jusqu'a la fin, puis ajoute le here_doc a la fin.

void	store_here_doc(t_minishell *vars, t_heredoc *new_heredoc)
{
	t_heredoc	*tmp;

	if (!vars->heredoc_list)
	{
		vars->heredoc_list = new_heredoc;
		return ;
	}
	tmp = vars->heredoc_list;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new_heredoc;
}

// le but de cette fonction est de creer un nouveau here_doc de stocker
// ce que l'utilisateur ecrit dans le terminal dans le fichier correspondant.
// Pour ce faire, elle lit ce que l'utilisateur ecrit dans le terminal avec
// gnl(0);, et ecrit la ligne dans le fichier correspondant avec ft_putstr_fd.
// Il faut aussi expand le delimiter si besoin car gngngngn bash.

int	get_here_doc_data(t_token *token, t_heredoc *hd, t_minishell *vars)
{
	char	*delimiter;
	char	*line;

	expand_delimiter(token->next, vars);
	delimiter = token->next->token;
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal_state == SIGINT)
		{
			if (g_signal_state != SIGINT)
				printf(HD_D_ERROR, token->next->token);
			vars->exit_value = 130;
			return (0);
		}
		if (!ft_strcmp(delimiter, line))
			break ;
		ft_putstr_fd(line, hd->fd);
		ft_putstr_fd("\n", hd->fd);
		free(line);
	}
	free(line);
	return (0);
}

// Cette fonction est appelee par get_here_doc_name pour trouver un nom
// et genere un nom de fichier pour le here_doc. Elle retourne le nom
// du fichier. J'avoue elle degueulasse, mais elle marche bien.

char	*generate_here_doc_name(char *name, int *size)
{
	int	i;

	i = -1;
	if (!name)
	{
		name = ft_malloc(sizeof(char) * ((*size) + 1));
		if (!name)
			return (NULL);
		name[++i] = '.';
		while (++i < (*size))
			name[i] = 'a';
		name[i] = '\0';
		return (name);
	}
	i++;
	while (name[i] == 'z' || name[i] == '.')
		i++;
	if (i == (*size - 1))
	{
		name = NULL;
		(*size)++;
		return (generate_here_doc_name(name, size));
	}
	name[i]++;
	return (name);
}

// le but de cette fonction est de, d'abord trouver un nom de fichier
// qui n'est pas deja utilise, puis quand elle trouve ouvre un fichier
// avec ce nom. Ensuite elle appelle get_here_doc_data pour ecrire dans
// ce fichier

char	*get_here_doc_name(t_token *token, t_heredoc *hd, t_minishell *vars)
{
	char	*hd_name_generated;
	int		size;

	hd_name_generated = NULL;
	size = 3;
	while (1)
	{
		hd_name_generated = generate_here_doc_name(hd_name_generated, &size);
		if (hd_name_generated == NULL)
			break ;
		hd->fd = open(hd_name_generated, O_CREAT | O_EXCL | O_RDWR, 0666);
		if (hd->fd > 2)
		{
			signal_handler(HEREDOC_MODE);
			get_here_doc_data(token, hd, vars);
			close(hd->fd);
			break ;
		}
	}
	return (hd_name_generated);
}

// le but de cette fonction est de creer un nouveau here_doc et de le stocker
// dans la liste de here_doc de la structure t_minishell, qui est une liste
// chainee de here_doc.

int	get_here_doc(t_minishell *vars, t_token *token)
{
	t_heredoc	*new_heredoc;
	char		*new_heredoc_name;

	vars->here_doc_counter++;
	if (vars->here_doc_counter == 16)
		return (printf("Michel: too many here_doc\n"));
	new_heredoc = ft_malloc(sizeof(t_heredoc));
	if (!new_heredoc)
		return (printf("Michel: malloc error\n"));
	new_heredoc->name = NULL;
	new_heredoc->next = NULL;
	new_heredoc_name = get_here_doc_name(token, new_heredoc, vars);
	if (new_heredoc_name == NULL)
		return (printf("Michel: malloc errora\n"));
	new_heredoc->name = new_heredoc_name;
	store_here_doc(vars, new_heredoc);
	if (g_signal_state == SIGINT)
	{
		if (g_signal_state == SIGINT)
			open("/dev/tty", O_RDONLY);
		vars->head = NULL;
		return (0);
	}
	g_signal_state = PROMPT_MODE;
	return (1);
}

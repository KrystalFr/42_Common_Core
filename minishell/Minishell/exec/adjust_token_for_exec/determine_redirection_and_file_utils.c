/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   determine_redirection_and_file_utils.c             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 22:29:23 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/17 13:22:35 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	is_std(char *str)
{
	if (!ft_strncmp(str, "/dev/stdout", 12) || !ft_strncmp(str, "/dev/stdin",
			10) || !ft_strncmp(str, "/dev/stderr", 11))
		return (true);
	return (false);
}

int	get_std_fd(char *str)
{
	if (!ft_strncmp(str, "/dev/stdout", 12))
		return (STDOUT_FILENO);
	if (!ft_strncmp(str, "/dev/stdin", 10))
		return (STDIN_FILENO);
	if (!ft_strncmp(str, "/dev/stderr", 11))
		return (STDERR_FILENO);
	return (-1);
}

void	open_infile(t_redirection *redirection)
{
	if (is_std(redirection->in_file))
	{
		if (access(redirection->in_file, F_OK | R_OK) == -1)
			redirection->in_fd = -1;
		else
			redirection->in_fd = get_std_fd(redirection->in_file);
		return ;
	}
	redirection->in_fd = open(redirection->in_file, O_RDONLY);
}

void	open_outfile(t_redirection *redirection)
{
	if (is_std(redirection->out_file))
	{
		if (access(redirection->out_file, F_OK | R_OK) == -1)
			redirection->out_fd = -1;
		else
			redirection->out_fd = get_std_fd(redirection->out_file);
		return ;
	}
	if (redirection->redirection_type == OUT_APPEND_REDIR)
		redirection->out_fd = open(redirection->out_file,
				O_WRONLY | O_APPEND | O_CREAT, 0644);
	else
		redirection->out_fd = open(redirection->out_file,
				O_WRONLY | O_TRUNC | O_CREAT, 0644);
}

void	close_file(int fd)
{
	if (fd > 2)
		close(fd);
}

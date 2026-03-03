/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk_bonus.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 14:39:36 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/29 19:06:55 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_BONUS_H
# define MINITALK_BONUS_H

# include "../../ft_Printf/ft_printf.h"
# include <unistd.h>
# include <signal.h>
# include <stdlib.h>
# include <stdio.h>

//__Utils__

int		ft_atoi(char *str);
int		ft_strlen(char *str);
size_t	ft_strlcpy(char *dest, char *src, size_t size);
void	error_msg(char *str);
int		ft_strcmp(char *s1, char *s2);

//__Client__

void	char_to_sig(int c, int pid);
void	handle_receipt(void);
void	msg_received(int sig);

//__Server__

void	sig_to_char(int sig, siginfo_t *sigb, void *arg);
char	*build_str(char c, char *str, siginfo_t *sibg);
void	print_msg(char *str, int pid);
#endif
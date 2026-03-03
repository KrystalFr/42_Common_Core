/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 13:28:23 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/29 19:10:51 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk_bonus.h"

char	*build_str(char c, char *str, siginfo_t *sibg)
{
	size_t	len;
	char	*tmp;

	if (!str)
		len = 0;
	else
		len = ft_strlen(str);
	tmp = malloc((len + 2) * sizeof(char));
	if (!tmp)
		error_msg("Malloc failed in build_str\n");
	if (str)
	{
		ft_strlcpy(tmp, str, len + 2);
		free(str);
	}
	tmp[len] = c;
	tmp[len + 1] = '\0';
	if (c == 0)
	{
		print_msg(tmp, sibg->si_pid);
		return (NULL);
	}
	else
		return (tmp);
}

void	sig_to_char(int sig, siginfo_t *sigb, void *arg)
{
	static int	nb = 8;
	static char	c = 0;
	static char	*str = NULL;

	(void)arg;
	nb--;
	if (sig == SIGUSR1)
	{
		c <<= 1;
		c |= 0b00000001;
	}
	if (sig == SIGUSR2)
		c <<= 1;
	if (!nb)
	{
		nb = 8;
		str = build_str(c, str, sigb);
		c = 0;
	}
}

void	print_msg(char *str, int pid)
{
	if (ft_strcmp(str, "/Exit") == 0)
	{
		free(str);
		exit(0);
	}
	else
	{
		kill(pid, SIGUSR1);
		ft_printf("%s\n", str);
		free(str);
	}
}

int	main(void)
{
	struct sigaction	sa_msg;

	sigemptyset(&sa_msg.sa_mask);
	sa_msg.sa_sigaction = sig_to_char;
	sa_msg.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &sa_msg, NULL) == -1)
		error_msg("SIGUSR1 failed\n");
	if (sigaction(SIGUSR2, &sa_msg, NULL) == -1)
		error_msg("SIGUSR2 failed\n");
	ft_printf("My PID is : %d\nTo close the server, type '/Exit'\n", getpid());
	while (1)
		sleep(10);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 13:28:27 by krfranco          #+#    #+#             */
/*   Updated: 2024/05/02 20:27:26 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk_bonus.h"

//SIGUSR1 = 1, SIGUSR2 = 0
void	char_to_sig(int c, int pid)
{
	int	bits;

	bits = 8;
	while (bits--)
	{
		if (c & 0b10000000)
			kill(pid, SIGUSR1);
		else
			kill(pid, SIGUSR2);
		c <<= 1;
		usleep(1000);
	}
}

void	msg_received(int sig)
{
	if (sig == SIGUSR1)
		ft_printf("Message received ✔\n");
	else
		ft_printf("Ratio\n");
	exit(0);
}

void	handle_receipt(void)
{
	struct sigaction	sa_receipt;

	sigemptyset(&sa_receipt.sa_mask);
	sa_receipt.sa_handler = msg_received;
	sa_receipt.sa_flags = SA_SIGINFO;
	if (sigaction(SIGUSR1, &sa_receipt, NULL) == -1)
		error_msg("SIGUSR1 receipt failed\n");
}

int	main(int ac, char **av)
{
	char				*msg;
	int					pid;
	int					i;

	if (ac != 3)
	{
		ft_printf("Need: %s <PID> <Message>\n", av[0]);
		return (1);
	}
	pid = ft_atoi(av[1]);
	msg = av[2];
	if (pid <= 0)
	{
		ft_printf("Need: %s <PID> <Message>\n", av[0]);
		return (1);
	}
	handle_receipt();
	i = 0;
	while (msg[i])
	{
		char_to_sig(msg[i], pid);
		i++;
	}
	char_to_sig(0, pid);
	return (0);
}

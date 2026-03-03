/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 13:47:38 by krfranco          #+#    #+#             */
/*   Updated: 2025/06/01 10:25:52 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	init_philo_utils(t_philo *philo, int i)
{
	philo->id = i + 1;
	philo->last_meal = time_phi();
	philo->as_eaten = 0;
}

int	init_philo(t_philo **philosophers, t_stat *data)
{
	long long	i;
	t_philo		*philo;

	i = -1;
	philo = *philosophers;
	philo = malloc(sizeof(t_philo) * data->nb_philo + 1);
	if (!philo)
		return (0);
	while (++i < data->nb_philo)
	{
		init_philo_utils(&philo[i], i);
		pthread_mutex_init(&philo[i].l_fork, NULL);
		if (i != data->nb_philo - 1)
			philo[i].r_fork = &philo[i + 1].l_fork;
		else
			philo[i].r_fork = &philo[0].l_fork;
		philo[i].data = data;
		if (pthread_mutex_init(&philo[i].m_philo, NULL) != 0)
			return (ft_error("Error\n"), 0);
	}
	*philosophers = philo;
	return (1);
}

int	init_stat(char **av, t_stat *data)
{
	data->nb_philo = ft_atoi(av[1]);
	data->die = ft_atoi(av[2]);
	data->eat = ft_atoi(av[3]);
	data->sleep = ft_atoi(av[4]);
	if (data->eat > data->sleep || ft_strlen(av[2]) > 4)
		data->think = data->eat - data->sleep;
	else
		data->think = (data->die - data->eat - data->sleep) / 2;
	data->m_eat = -1;
	if (av[5])
		data->m_eat = ft_atoi(av[5]);
	data->philo_died = 0;
	data->all_ate = 0;
	if (data->nb_philo == 0 || data->m_eat == 0 || data->die == 0)
		return (0);
	if (pthread_mutex_init(&data->print, NULL))
		return (0);
	if (pthread_mutex_init(&data->dead, NULL))
		return (0);
	return (1);
}

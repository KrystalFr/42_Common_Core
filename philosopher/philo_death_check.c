/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_death_check.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 14:32:16 by krfranco          #+#    #+#             */
/*   Updated: 2025/06/01 01:39:58 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	mutex_all_eat(t_stat *data)
{
	pthread_mutex_lock(&(data->dead));
	data->all_ate = 1;
	pthread_mutex_unlock(&(data->dead));
	return (1);
}

int	check_nb_eat(t_stat *data, t_philo *philo)
{
	int	i;

	i = data->nb_philo - 1;
	pthread_mutex_lock(&(data->dead));
	if (data->philo_died == 1)
	{
		pthread_mutex_unlock(&(data->dead));
		return (1);
	}
	pthread_mutex_unlock(&(data->dead));
	if (data->m_eat == -1)
		return (0);
	i = data->nb_philo;
	while (--i >= 0)
	{
		pthread_mutex_lock(&(philo[i].m_philo));
		if (philo[i].as_eaten < data->m_eat)
		{
			pthread_mutex_unlock(&(philo[i].m_philo));
			return (0);
		}
		pthread_mutex_unlock(&(philo[i].m_philo));
	}
	mutex_all_eat(data);
	return (1);
}

int	mutex_dead(t_stat *data, t_philo *philo)
{
	print (time_phi(), philo, "died");
	pthread_mutex_lock(&(data->dead));
	data -> philo_died = 1;
	pthread_mutex_unlock(&(data->dead));
	return (1);
}

int	check_data_died(t_stat *data)
{
	pthread_mutex_lock(&(data->dead));
	if (data->philo_died == 1 || data->all_ate == 1)
	{
		pthread_mutex_unlock(&(data->dead));
		return (1);
	}
	pthread_mutex_unlock(&(data->dead));
	return (0);
}

void	ft_monitoring(t_stat *data, t_philo *philo)
{
	int	i;

	while (check_nb_eat(data, philo) == 0)
	{
		i = -1;
		while (++i < data -> nb_philo)
		{
			pthread_mutex_lock(&(philo[i].m_philo));
			if ((time_phi() - philo[i].last_meal) > data->die)
			{
				mutex_dead(data, &philo[i]);
				pthread_mutex_unlock(&(philo[i].m_philo));
				break ;
			}
			pthread_mutex_unlock(&(philo[i].m_philo));
		}
		usleep(500);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umartin- <umartin-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/15 17:28:59 by umartin-          #+#    #+#             */
/*   Updated: 2022/12/19 13:25:06 by umartin-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	philo_dead(t_philo *philo, int time)
{
	int	current;

	current = get_time();
	if (philo->data->death == 1)
		return (1);
	if ((current - philo->last_meal) > philo->data->time_to_die)
	{
		philo->data->death = 1;
		pthread_mutex_lock(&philo->data->write_mutex);
		printf("\x1B[34m%d\x1B[0m  %d \033[1;37mdied 💀\033[0;m\n",
			time, philo->num + 1);
		return (1);
	}
	return (0);
}

int	routine_sleep(t_philo	*philo)
{
	if (philo_dead(philo, time_clock(philo->data->init_time)) == 1)
		return (1);
	ft_write(philo, "is sleeping", time_clock(philo->data->init_time));
	if (precise_usleep(philo->data->time_to_eat, philo) == 1)
		return (1);
	return (0);
}

int	routine_think(t_philo	*philo)
{
	if (philo_dead(philo, time_clock(philo->data->init_time)) == 1)
		return (1);
	ft_write(philo, "is thinking", time_clock(philo->data->init_time));
	return (0);
}

int	routine_eat(t_philo	*philo)
{
	while (philo->data->fork[philo->left_fork].lck == 1)
		if (philo_dead(philo, time_clock(philo->data->init_time)) == 1)
			return (1);
	pthread_mutex_lock(&philo->data->fork[philo->left_fork].fork_th);
	philo->data->fork[philo->left_fork].lck = 1;
	ft_write(philo, "has taken a fork", time_clock(philo->data->init_time));
	while (philo->data->fork[philo->right_fork].lck == 1)
		if (philo_dead(philo, time_clock(philo->data->init_time)) == 1)
			return (1);
	pthread_mutex_lock(&philo->data->fork[philo->right_fork].fork_th);
	philo->data->fork[philo->right_fork].lck = 1;
	ft_write(philo, "has taken a fork", time_clock(philo->data->init_time));
	philo->last_meal = get_time();
	ft_write(philo, "is eating", time_clock(philo->data->init_time));
	if (precise_usleep(philo->data->time_to_eat, philo) == 1)
		return (1);
	pthread_mutex_unlock(&philo->data->fork[philo->left_fork].fork_th);
	philo->data->fork[philo->left_fork].lck = 0;
	pthread_mutex_unlock(&philo->data->fork[philo->right_fork].fork_th);
	philo->data->fork[philo->right_fork].lck = 0;
	return (0);
}

void	*control_de_rutina(void *data)
{
	t_philo	*philo;

	philo = (t_philo *)data;
	if (philo->num % 2 == 0)
		usleep (800);
	philo->last_meal = get_time();
	while (1)
	{
		if (routine_eat(philo) == 1)
		{
			pthread_mutex_unlock(&philo->data->write_mutex);
			return (NULL);
		}
		if (routine_sleep(philo) == 1)
		{
			pthread_mutex_unlock(&philo->data->write_mutex);
			return (NULL);
		}
		if (routine_think(philo) == 1)
		{
			pthread_mutex_unlock(&philo->data->write_mutex);
			return (NULL);
		}
	}
}

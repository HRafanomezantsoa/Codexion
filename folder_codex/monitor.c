/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 10:34:36 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 13:36:46 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wake_all(t_sim *sim)
{
	int	j;

	j = 0;
	while (j < sim->args->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[j].mutex);
		pthread_cond_broadcast(&sim->dongles[j].cond);
		pthread_mutex_unlock(&sim->dongles[j].mutex);
		j++;
	}
}

static int	check_burnout(t_sim *sim, long long now)
{
	int	i;

	i = 0;
	while (i < sim->args->number_of_coders)
	{
		pthread_mutex_lock(&sim->state_mutex);
		if ((sim->coders[i].compiles_done) < (
				sim->args->number_of_compiles_required)
			&& now >= sim->coders[i].deadline)
		{
			sim->running = 0;
			pthread_mutex_unlock(&sim->state_mutex);
			log_state(sim, sim->coders[i].id + 1, "burned out");
			wake_all(sim);
			return (1);
		}
		pthread_mutex_unlock(&sim->state_mutex);
		i++;
	}
	return (0);
}

static int	check_finished(t_sim *sim)
{
	int	i;
	int	done;

	pthread_mutex_lock(&sim->state_mutex);
	done = 1;
	i = 0;
	while (i < sim->args->number_of_coders)
	{
		if ((sim->coders[i].compiles_done) < (
				sim->args->number_of_compiles_required))
		{
			done = 0;
			break ;
		}
		i++;
	}
	if (done)
	{
		sim->running = 0;
		pthread_mutex_unlock(&sim->state_mutex);
		wake_all(sim);
		return (1);
	}
	pthread_mutex_unlock(&sim->state_mutex);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	long long	now;

	sim = (t_sim *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->state_mutex);
		if (!sim->running)
		{
			pthread_mutex_unlock(&sim->state_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->state_mutex);
		now = get_time_ms();
		if (check_burnout(sim, now))
			return (NULL);
		if (check_finished(sim))
			break ;
		usleep(1000);
	}
	return (NULL);
}

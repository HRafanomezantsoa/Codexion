/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 10:32:15 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 13:14:09 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_running(t_sim *sim)
{
	int	running;

	pthread_mutex_lock(&sim->state_mutex);
	running = sim->running;
	pthread_mutex_unlock(&sim->state_mutex);
	return (running);
}

static int	take_two_dongles(t_dongle *first, t_dongle *second,
							t_coder *coder, t_sim *sim)
{
	if (!dongle_take(first, coder, sim))
		return (0);
	log_state(sim, coder->id + 1, "has taken a dongle");
	if (!dongle_take(second, coder, sim))
	{
		dongle_release(first, sim);
		return (0);
	}
	log_state(sim, coder->id + 1, "has taken a dongle");
	return (1);
}

static int	take_dongles(t_coder *coder, t_sim *sim)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left_dongle == coder->right_dongle)
	{
		if (!dongle_take(coder->left_dongle, coder, sim))
			return (0);
		log_state(sim, coder->id + 1, "has taken a dongle");
		return (1);
	}
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	return (take_two_dongles(first, second, coder, sim));
}

static int	do_compile(t_coder *coder, t_sim *sim)
{
	pthread_mutex_lock(&sim->state_mutex);
	coder->last_compile_start = get_time_ms();
	coder->deadline = coder->last_compile_start + sim->args->time_to_burnout;
	pthread_mutex_unlock(&sim->state_mutex);
	log_state(sim, coder->id + 1, "is compiling");
	usleep(sim->args->time_to_compile * 1000);
	if (!check_running(sim))
		return (0);
	dongle_release(coder->left_dongle, sim);
	if (coder->left_dongle != coder->right_dongle)
		dongle_release(coder->right_dongle, sim);
	pthread_mutex_lock(&sim->state_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&sim->state_mutex);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (1)
	{
		if (!check_running(sim))
			break ;
		if (coder->compiles_done >= sim->args->number_of_compiles_required)
			break ;
		if (!take_dongles(coder, sim))
			break ;
		if (!do_compile(coder, sim))
			break ;
		if (!do_debug_and_refactor(coder, sim))
			break ;
	}
	return (NULL);
}

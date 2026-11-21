/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 10:26:00 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 12:24:49 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_request	heap_keep(t_heap *heap)
{
	t_request	empty;

	empty = (t_request){-1, 0, 0};
	if (heap->size == 0)
	{
		return (empty);
	}
	return (heap->data[0]);
}

static void	get_timeout(struct timespec *ts, struct timeval *tv)
{
	gettimeofday(tv, NULL);
	ts->tv_sec = tv->tv_sec;
	ts->tv_nsec = (tv->tv_usec + 1000) * 1000;
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec += 1;
		ts->tv_nsec -= 1000000000;
	}
}

static int	wait_for_dongle(t_dongle *dongle, t_coder *coder, t_sim *sim)
{
	struct timespec	ts;
	struct timeval	tv;

	while (sim->running)
	{
		if (dongle->available && get_time_ms() >= dongle->cooldown_end
			&& heap_keep(&dongle->heap).coder_id == coder->id)
			return (1);
		get_timeout(&ts, &tv);
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	return (0);
}

int	dongle_take(t_dongle *dongle, t_coder *coder, t_sim *sim)
{
	long long		now;
	t_request		req;

	pthread_mutex_lock(&dongle->mutex);
	now = get_time_ms();
	if (dongle->available && (now >= dongle->cooldown_end))
	{
		dongle->available = 0;
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	req.coder_id = coder->id;
	req.timestamp = now;
	req.deadline = coder->deadline;
	heap_push(&dongle->heap, req, sim->args->scheduler);
	if (!wait_for_dongle(dongle, coder, sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	heap_pop(&dongle->heap, sim->args->scheduler);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

void	dongle_release(t_dongle *dongle, t_sim *sim)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = 1;
	dongle->cooldown_end = get_time_ms() + sim->args->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

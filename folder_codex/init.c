/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/20 07:58:44 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 13:24:22 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *heap, int capacity)
{
	heap->data = malloc(sizeof(t_request) * capacity);
	if (!heap->data)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	return (1);
}

static int	init_dongle(t_dongle *dongle, int id, int capacity)
{
	dongle->id = id;
	dongle->available = 1;
	dongle->cooldown_end = 0;
	pthread_mutex_init(&dongle->mutex, NULL);
	pthread_cond_init(&dongle->cond, NULL);
	if (!heap_init(&dongle->heap, capacity))
		return (0);
	return (1);
}

static void	init_coders(t_sim *sim, t_args *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		sim->coders[i].id = i;
		sim->coders[i].state = coder_waiting;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[
			(i + 1) % args->number_of_coders];
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].deadline = sim->start_time + args->time_to_burnout;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].sim = sim;
		i++;
	}
}

static int	init_all_dongles(t_sim *sim, t_args *args)
{
	int	i;
	int	j;

	i = 0;
	while (i < args->number_of_coders)
	{
		if (!init_dongle(&sim->dongles[i], i, args->number_of_coders))
		{
			j = 0;
			while (j < i)
			{
				free(sim->dongles[j].heap.data);
				pthread_mutex_destroy(&sim->dongles[j].mutex);
				pthread_cond_destroy(&sim->dongles[j].cond);
				j++;
			}
			free(sim->coders);
			sim->coders = NULL;
			free(sim->dongles);
			sim->dongles = NULL;
			return (0);
		}
		i++;
	}
	return (1);
}

int	init_simulation(t_sim *sim, t_args *args)
{
	sim->args = args;
	sim->dongles = malloc(sizeof(t_dongle) * args->number_of_coders);
	if (!sim->dongles)
		return (0);
	sim->coders = malloc(sizeof(t_coder) * args->number_of_coders);
	if (!sim->coders)
	{
		free(sim->dongles);
		sim->dongles = NULL;
		return (0);
	}
	sim->running = 1;
	sim->start_time = get_time_ms();
	if (!init_all_dongles(sim, args))
		return (0);
	init_coders(sim, args);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
	return (1);
}

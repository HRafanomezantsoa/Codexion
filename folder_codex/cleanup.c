/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 10:33:20 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 13:38:40 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	cleanup_simulation(t_sim *sim)
{
	int	i;

	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	i = 0;
	while (i < sim->args->number_of_coders)
	{
		free(sim->dongles[i].heap.data);
		sim->dongles[i].heap.data = NULL;
		i++;
	}
	i = 0;
	while (i < sim->args->number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
	free(sim->coders);
	sim->coders = NULL;
	free(sim->dongles);
	sim->dongles = NULL;
}

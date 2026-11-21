/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 23:14:35 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/20 07:49:02 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	create_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			sim->running = 0;
			while (i > 0)
			{
				i--;
				pthread_join(sim->coders[i].thread, NULL);
			}
			return (1);
		}
		i++;
	}
	return (0);
}

static void	join_pthreads(t_sim *sim, pthread_t monitor_thread)
{
	int	i;

	i = 0;
	pthread_join(monitor_thread, NULL);
	while (i < sim->args->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	main(int ac, char **av)
{
	t_args		args;
	t_sim		sim;
	pthread_t	monitor_thread;

	if (ac != 9)
		return (1);
	if (!valid_args(av, &args))
		return (1);
	if (!init_simulation(&sim, &args))
		return (1);
	if (create_coders(&sim))
	{
		cleanup_simulation(&sim);
		return (1);
	}
	if (pthread_create(&monitor_thread, NULL, monitor_routine, &sim) != 0)
	{
		sim.running = 0;
		cleanup_simulation(&sim);
		return (1);
	}
	join_pthreads(&sim, monitor_thread);
	cleanup_simulation(&sim);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 10:43:19 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 13:20:48 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + tv.tv_usec / 1000);
}

void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	long long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	timestamp = get_time_ms() - sim->start_time;
	printf("%lld %d %s\n", timestamp, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

int	do_debug_and_refactor(t_coder *coder, t_sim *sim)
{
	log_state(sim, coder->id + 1, "is debugging");
	usleep(sim->args->time_to_debug * 1000);
	if (!check_running(sim))
		return (0);
	log_state(sim, coder->id + 1, "is refactoring");
	usleep(sim->args->time_to_refactor * 1000);
	if (!check_running(sim))
		return (0);
	return (1);
}

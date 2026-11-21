/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 10:40:17 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 12:52:11 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_number(char *str)
{
	int	i;

	i = 0;
	if (!(str[i]))
		return (0);
	while (str[i])
	{
		if (!((str[i] >= '0') && (str[i] <= '9')))
			return (0);
		i++;
	}
	return (1);
}

int	parse_int(char *str, int *result)
{
	int	i;
	int	d;

	i = 0;
	d = 0;
	*result = 0;
	while (str[i])
	{
		d = str[i] - '0';
		if (*result > INT_MAX / 10 || (
				*result == INT_MAX / 10 && d > INT_MAX % 10))
			return (0);
		*result = *result * 10 + d;
		i++;
	}
	return (1);
}

static int	parse_sheduler(char *str, t_args *args)
{
	if (strcmp(str, "fifo") == 0)
		args->scheduler = FIFO;
	else if (strcmp(str, "edf") == 0)
		args->scheduler = EDF;
	else
		return (0);
	return (1);
}

int	valid_args(char **av, t_args *args)
{
	int	*fields[7];
	int	i;

	fields[0] = &args->number_of_coders;
	fields[1] = &args->time_to_burnout;
	fields[2] = &args->time_to_compile;
	fields[3] = &args->time_to_debug;
	fields[4] = &args->time_to_refactor;
	fields[5] = &args->number_of_compiles_required;
	fields[6] = &args->dongle_cooldown;
	i = 0;
	while (i < 7)
	{
		if (!is_number(av[i + 1]))
			return (0);
		if (!parse_int(av[i + 1], fields[i]))
			return (0);
		i++;
	}
	return (parse_sheduler(av[8], args));
}

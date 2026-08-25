/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:58:19 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/25 14:07:22 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

static int	is_number(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	parse_arguments(int argc, char **argv, t_simulator *sim)
{
	int	i;

	if (argc != 9)
		return (0);
	i = 1;
	while (i <= 7)
	{
		if (!is_number(argv[i]) || atoi(argv[i]) <= 0)
			return (0);
		i++;
	}
	if (strcmp(argv[8], "fifo") == 0)
		sim->schedule_type = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->schedule_type = EDF;
	else
		return (0);
	sim->nb_coder = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refrator = atoi(argv[5]);
	sim->nb_compilation_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	return (1);
}

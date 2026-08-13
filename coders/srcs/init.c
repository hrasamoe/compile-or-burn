/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 13:38:48 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 15:06:05 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

static t_coder	*init_coder(t_simulator *simulation)
{
	int			i;
	t_coder		*coder_array;
	long long	start_time;

	coder_array = malloc(sizeof(t_coder) * simulation->nb_coder);
	if (!coder_array)
		return (NULL);
	i = 0;
	start_time = get_current_time();
	while (i < simulation->nb_coder)
	{
		pthread_mutex_init(&coder_array[i].lock, NULL);
		coder_array[i].deadline = start_time + simulation->time_to_burnout;
		coder_array[i].id = i + 1;
		coder_array[i].state = THINKING;
		coder_array[i].nb_compilation = 0;
		coder_array[i].last_compilation = start_time;
		coder_array[i].simulator = simulation;
		coder_array[i].dongle_left = &simulation->dongles[i];
		coder_array[i].dongle_right = &simulation->dongles[(i + 1)
			% simulation->nb_coder];
		i++;
	}
	return (coder_array);
}

static t_dongle	*init_dongles(t_simulator *simulation)
{
	t_dongle	*dongle_array;
	int			i;

	dongle_array = malloc(sizeof(t_dongle) * simulation->nb_coder);
	if (!dongle_array)
		return (NULL);
	while (i < simulation->nb_coder)
	{
		dongle_array[i].held_by = -1;
		dongle_array[i].id = i + 1;
		dongle_array[i].is_available = 1;
		dongle_array[i].unvailable_until = 0;
		if (pthread_mutex_init(&dongle_array[i].lock, NULL) != 0)
			return (free_dongle_on_error(dongle_array, i), NULL);
		if (pthread_cond_init(&dongle_array[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&dongle_array[i]);
			return (free_dongle_on_error(dongle_array, i), NULL);
		}
		i++;
	}
	return (dongle_array);
}

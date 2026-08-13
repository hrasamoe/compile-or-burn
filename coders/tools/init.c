/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/13 13:38:48 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 14:12:09 by hrasamoe         ###   ########.fr       */
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 13:38:48 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 15:41:39 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

static t_heap	*heap_init(t_simulator *simulation)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->request_array = malloc(sizeof(t_request) * simulation->nb_coder);
	if (!heap->request_array)
	{
		free(heap);
		return (NULL);
	}
	heap->size = 0;
	heap->capacity = simulation->nb_coder;
	heap->schedule_type = simulation->schedule_type;
	pthread_mutex_init(&heap->lock, NULL);
	return (heap);
}

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

int	init_simulation(t_simulator *simulation)
{
	simulation->start_time = get_current_time();
	simulation->stop = 0;
	if (pthread_mutex_init(&simulation->stop_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&simulation->heap_lock, NULL) != 0)
		return (pthread_mutex_destroy(&simulation->stop_lock), 0);
	if (pthread_mutex_init(&simulation->log_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->stop_lock);
		pthread_mutex_destroy(&simulation->heap_lock);
		return (0);
	}
	simulation->request_heap = heap_init(simulation);
	if (!simulation->request_heap)
		return (clean_simulation(simulation), 0);
	simulation->dongles = init_dongles(simulation);
	if (!simulation->dongles)
		return (clean_simulation(simulation), 0);
	simulation->coder = init_coder(simulation);
	if (!simulation->coder)
		return (clean_simulation(simulation), 0);
	return (1);
}

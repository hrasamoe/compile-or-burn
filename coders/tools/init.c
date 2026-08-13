/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 13:38:48 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 15:41:11 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	free_dongle_on_error(t_dongle *dongles, int i)
{
	int		j;

	j = 0;
	while (j < i)
	{
		pthread_mutex_destroy(&dongles[j].lock);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
	free(dongles);
}

static void	destroy_heap(t_simulator *simulation)
{
	if (!simulation->request_heap)
		return ;
	pthread_mutex_destroy(&simulation->request_heap->lock);
	free(simulation->request_heap->request_array);
	free(simulation->request_heap);
}

static void	free_dongles(t_simulator *simulation)
{
	int		i;

	if (!simulation->dongles)
		return ;
	while (i < simulation->nb_coder)
	{
		pthread_mutex_destroy(&simulation->dongles[i].lock);
		pthread_cond_destroy(&simulation->dongles[i].cond);
		i++;
	}
	free(simulation->dongles);
	simulation->dongles = NULL;
}

static void	free_coder(t_simulator *simulation)
{
	int	i;

	if (simulation->coder == NULL)
		return ;
	i = 0;
	while (i < simulation->nb_coder)
	{
		pthread_mutex_destroy(&simulation->coder[i].lock);
		i++;
	}
	free(simulation->coder);
	simulation->coder = NULL;
}

void	clean_simulation(t_simulator *simulation)
{
	free_dongles(simulation);
	free_coder(simulation);
	if (simulation->request_heap)
	{
		destroy_heap(simulation);
		simulation->request_heap = NULL;
	}
	pthread_mutex_destroy(&simulation->log_lock);
	pthread_mutex_destroy(&simulation->heap_lock);
	pthread_mutex_destroy(&simulation->stop_lock);
}

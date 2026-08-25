/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 12:35:45 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/25 15:25:58 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

int	compare_heap(t_request a, t_request b, t_schedule schedule_type)
{
	if (schedule_type == FIFO)
	{
		if (a.arrival_time != b.arrival_time)
		{
			if (a.arrival_time < b.arrival_time)
				return (-1);
			else
				return (1);
		}
		return (a.coder_id - b.coder_id);
	}
	else
	{
		if (a.deadline != b.deadline)
		{
			if (a.deadline < b.deadline)
				return (-1);
			else
				return (1);
		}
		return (a.coder_id - b.coder_id);
	}
}

int	push_heap(t_heap *heap, t_request new_request)
{
	int			i;
	t_request	priority_request;

	if (!heap)
		return (0);
	pthread_mutex_lock(&heap->lock);
	if (heap->size >= heap->capacity)
	{
		pthread_mutex_unlock(&heap->lock);
		return (0);
	}
	i = heap->size;
	heap->request_array[i] = new_request;
	heap->size++;
	while (i > 0 && compare_heap(heap->request_array[i],
			heap->request_array[(i - 1) / 2], heap->schedule_type) < 0)
	{
		priority_request = heap->request_array[i];
		heap->request_array[i] = heap->request_array[(i - 1) / 2];
		heap->request_array[(i - 1) / 2] = priority_request;
		i = (i - 1) / 2;
	}
	pthread_mutex_unlock(&heap->lock);
	return (1);
}

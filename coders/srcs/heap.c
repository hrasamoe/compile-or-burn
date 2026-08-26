/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 15:23:53 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/26 13:26:34 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

static int	compare_heap_child(t_heap *heap, int i)
{
	int	child;

	child = 2 * i + 1;
	if (child + 1 < heap->size && compare_heap(heap->request_array[child + 1],
			heap->request_array[child], heap->schedule_type) < 0)
		child = child + 1;
	return (child);
}

void	heap_shift(t_heap *heap, int i)
{
	int			child;
	t_request	request_temp;

	while (2 * i + 1 < heap->size)
	{
		child = compare_heap_child(heap, i);
		if (compare_heap(heap->request_array[i],
				heap->request_array[child],
				heap->schedule_type) <= 0)
			break ;
		request_temp = heap->request_array[child];
		heap->request_array[child] = heap->request_array[i];
		heap->request_array[i] = request_temp;
		i = child;
	}
}

t_request	*heap_pop(t_heap *heap)
{
	t_request	*result;

	if (!heap)
		return (0);
	result = malloc(sizeof(t_request));
	if (result == NULL)
		return (NULL);
	pthread_mutex_lock(&heap->lock);
	if (heap->size == 0)
	{
		free(result);
		pthread_mutex_unlock(&heap->lock);
		return (NULL);
	}
	*result = heap->request_array[0];
	heap->request_array[0] = heap->request_array[heap->size - 1];
	heap->size--;
	heap_shift(heap, 0);
	pthread_mutex_unlock(&heap->lock);
	return (result);
}

t_request	*peek_heap(t_heap *heap)
{
	t_request	*result;

	if (!heap)
		return (NULL);
	result = malloc(sizeof(t_request));
	if (result == NULL)
		return (NULL);
	pthread_mutex_lock(&heap->lock);
	if (heap->size == 0)
	{
		free(result);
		pthread_mutex_unlock(&heap->lock);
		return (NULL);
	}
	*result = heap->request_array[0];
	return (pthread_mutex_unlock(&heap->lock), result);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:06:54 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/09/04 14:41:33 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	take_dongles(t_coder *coder,
			t_dongle *dongle_left,
			t_dongle *dongle_right)
{
	if (should_stop(coder->simulator))
		return ;
	if (dongle_left == dongle_right)
		pthread_mutex_lock(&dongle_left->lock);
	else if (dongle_left->id < dongle_right->id)
	{
		pthread_mutex_lock(&dongle_left->lock);
		pthread_mutex_lock(&dongle_right->lock);
	}
	else
	{
		pthread_mutex_lock(&dongle_right->lock);
		pthread_mutex_lock(&dongle_left->lock);
	}
	print_log(coder, "has taken a dongle");
	if (dongle_left != dongle_right)
		print_log(coder, "has taken a dongle");
	dongle_left->is_available = 0;
	dongle_right->is_available = 0;
	dongle_left->held_by = coder->id;
	dongle_right->held_by = coder->id;
}

void	release_dongles(t_dongle *dongle_left,
			t_dongle *dongle_right,
			t_simulator *simulator)
{
	long	timestamp;

	timestamp = get_current_time();
	dongle_left->held_by = -1;
	dongle_left->is_available = 1;
	dongle_left->unavailable_until = timestamp + simulator->dongle_cooldown;
	pthread_mutex_unlock(&dongle_left->lock);
	if (dongle_left != dongle_right)
	{
		dongle_right->held_by = -1;
		dongle_right->is_available = 1;
		dongle_right->unavailable_until = timestamp
			+ simulator->dongle_cooldown;
		pthread_mutex_unlock(&dongle_right->lock);
	}
	pthread_mutex_lock(&simulator->alloc_lock);
	pthread_cond_broadcast(&simulator->alloc_cond);
	pthread_mutex_unlock(&simulator->alloc_lock);
}

int	heap_try_pop_if_mine(t_heap *heap, int coder_id, t_request *result)
{
	if (!heap || !result)
		return (0);
	pthread_mutex_lock(&heap->lock);
	if (heap->size == 0 || heap->request_array[0].coder_id != coder_id)
	{
		pthread_mutex_unlock(&heap->lock);
		return (0);
	}
	*result = heap->request_array[0];
	heap->request_array[0] = heap->request_array[heap->size - 1];
	heap->size--;
	heap_shift(heap, 0);
	pthread_mutex_unlock(&heap->lock);
	return (1);
}

void	acquire_dongles(t_coder *coder)
{
	t_request	new_request;
	t_request	popped_request;

	new_request.coder_id = coder->id;
	new_request.deadline = coder->last_compilation
		+ coder->simulator->time_to_burnout;
	new_request.arrival_time = get_current_time();
	push_heap(coder->simulator->request_heap, new_request);
	pthread_mutex_lock(&coder->simulator->alloc_lock);
	while (!should_stop(coder->simulator))
	{
		if (are_dongles_ready(coder->dongle_left, coder->dongle_right)
			&& heap_try_pop_if_mine(coder->simulator->request_heap,
				coder->id, &popped_request))
		{
			pthread_mutex_unlock(&coder->simulator->alloc_lock);
			take_dongles(coder, coder->dongle_left, coder->dongle_right);
			return ;
		}
		pthread_mutex_unlock(&coder->simulator->alloc_lock);
		usleep(1000);
		pthread_mutex_lock(&coder->simulator->alloc_lock);
	}
	pthread_mutex_unlock(&coder->simulator->alloc_lock);
}

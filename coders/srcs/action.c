/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:06:54 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/26 11:06:24 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	take_dongles(t_coder *coder,
			t_dongle *dongle_left,
			t_dongle *dongle_right)
{
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
		pthread_mutex_lock(&dongle_right->lock);
	}
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
	dongle_left->unvailable_until = timestamp + simulator->dongle_cooldown;
	dongle_right->held_by = -1;
	dongle_right->is_available = 1;
	dongle_right->unvailable_until = timestamp + simulator->dongle_cooldown;
}

void	aquire_dongles(t_coder *coder)
{
	t_request	new_request;
	t_request	*top_request;

	new_request.coder_id = coder->id;
	new_request.deadline = coder->last_compilation
		+ coder->simulator->time_to_burnout;
	new_request.arrival_time = get_current_time();
	push_heap(coder->simulator->request_heap, new_request);
	while (!should_stop(coder->simulator))
	{
		top_request = peek_heap(coder->simulator->request_heap);
		if (top_request->coder_id == coder->id
			&& are_dongles_ready(coder->dongle_left, coder->dongle_right))
		{
			heap_pop(&coder->simulator->heap_lock);
			free(top_request);
			take_dongles(coder, coder->dongle_left, coder->dongle_right);
			return ;
		}
		free(top_request);
		usleep(500);
	}
}

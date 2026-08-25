/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:06:54 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/25 14:03:47 by hrasamoe         ###   ########.fr       */
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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 13:38:48 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/25 15:20:40 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	set_stop_flag(t_simulator *simulation)
{
	pthread_mutex_lock(&simulation->stop_lock);
	simulation->stop = 1;
	pthread_mutex_lock(&simulation->stop_lock);
}

long long	get_current_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

static int	is_dongle_ready(t_dongle *dongle)
{
	long	now;

	now = get_current_time();
	return (dongle->is_available && now > dongle->unvailable_until);
}

int	are_dongles_ready(t_dongle *dongle_left, t_dongle *dongle_right)
{
	if (dongle_left == dongle_right)
		return (is_dongle_ready(dongle_left));
	return (is_dongle_ready(dongle_left) && is_dongle_ready(dongle_right));
}

void	precise_sleep(t_simulator *simulation, long duration)
{
	long	start;

	start = get_current_time();
	while (!should_stop(simulation))
	{
		if (get_current_time() - start >= duration)
			break ;
		usleep(500);
	}
}

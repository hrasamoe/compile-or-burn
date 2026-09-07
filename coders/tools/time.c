/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 12:05:54 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/09/07 12:06:32 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

long long	get_current_time(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000LL));
}

void	set_timeout(struct timespec *ts, long ms)
{
	clock_gettime(CLOCK_REALTIME, ts);
	ts->tv_nsec += ms * 1000000L;
	if (ts->tv_nsec >= 1000000000L)
	{
		ts->tv_sec += 1;
		ts->tv_nsec -= 1000000000L;
	}
}

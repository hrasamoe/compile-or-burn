/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 12:55:50 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/26 14:36:59 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	print_log(t_coder *coder, const char *action)
{
	long	timestamp;

	pthread_mutex_lock(&coder->simulator->log_lock);
	if (!should_stop(coder->simulator))
	{
		timestamp = get_current_time() - coder->simulator->start_time;
		printf("%ld %d %s\n", timestamp, coder->id, action);
	}
	pthread_mutex_unlock(&coder->simulator->log_lock);
}

void	set_stop_flag(t_simulator *simulation)
{
	pthread_mutex_lock(&simulation->stop_lock);
	simulation->stop = 1;
	pthread_mutex_unlock(&simulation->stop_lock);
}

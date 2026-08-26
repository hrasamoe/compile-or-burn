/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 12:55:50 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/26 13:37:56 by hrasamoe         ###   ########.fr       */
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

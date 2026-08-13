/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 13:38:48 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 15:03:45 by hrasamoe         ###   ########.fr       */
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

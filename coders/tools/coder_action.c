/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_action.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 14:22:26 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/09/04 13:53:04 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	coder_debug(t_coder *coder)
{
	if (should_stop(coder->simulator))
		return ;
	coder->state = DEBUGGING;
	print_log(coder, "is debugging");
	precise_sleep(coder->simulator, coder->simulator->time_to_debug);
}

void	coder_compile(t_coder *coder)
{
	if (should_stop(coder->simulator))
		return ;
	pthread_mutex_lock(&coder->lock);
	print_log(coder, "is compiling");
	coder->last_compilation = get_current_time();
	coder->state = COMPILING;
	pthread_mutex_unlock(&coder->lock);
	precise_sleep(coder->simulator, coder->simulator->time_to_compile);
	pthread_mutex_lock(&coder->lock);
	coder->nb_compilation++;
	pthread_mutex_unlock(&coder->lock);
}

void	coder_refactor(t_coder *coder)
{
	if (should_stop(coder->simulator))
		return ;
	coder->state = REFACTORING;
	print_log(coder, "is refactoring");
	precise_sleep(coder->simulator, coder->simulator->time_to_refactor);
}

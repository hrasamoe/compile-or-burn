/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 12:55:50 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/29 23:12:54 by hrasamoe         ###   ########.fr       */
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

void	print_start_msg(void)
{
	printf("\n\033[1;32m+--------------------------------"
		"-----------------+\033[0m\n");
	printf("\033[1;32m|        Starting the compilation"
		" process...      |\033[0m\n");
	printf("\033[1;32m+--------------------------------"
		"-----------------+\033[0m\n");
}

void	print_finish_msg(t_simulator *sim)
{
	usleep(300);
	printf("\033[1;32m+--------------------------------"
		"-----------------+\033[0m\n");
	printf("\033[1;32m|  All the coders have done their"
		" compilation     |\033[0m\n");
	printf("\033[1;32m+--------------------------------"
		"-----------------+\033[0m\n");
	set_stop_flag(sim);
}

void	print_err_msg(const char *msg)
{
	int	len;
	int	i;

	len = strlen(msg);
	i = 0;
	fprintf(stderr, "\033[1;31m+");
	while (i < len + 2)
	{
		fprintf(stderr, "-");
		i++;
	}
	fprintf(stderr, "+\033[0m\n");
	fprintf(stderr, "\033[1;31m| %s |\033[0m\n", msg);
	i = 0;
	fprintf(stderr, "\033[1;31m+");
	while (i < len + 2)
	{
		fprintf(stderr, "-");
		i++;
	}
	fprintf(stderr, "+\033[0m\n");
}

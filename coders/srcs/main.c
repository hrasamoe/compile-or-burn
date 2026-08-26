/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 12:58:13 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/26 13:23:42 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	set_stop_flag(t_simulator *simulation)
{
	pthread_mutex_lock(&simulation->stop_lock);
	simulation->stop = 1;
	pthread_mutex_unlock(&simulation->stop_lock);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!should_stop(coder->simulator)
		&& coder->nb_compilation < coder->simulator->nb_compilation_required)
	{
		aquire_dongles(coder);
		coder_compile(coder);
		release_dongles(coder->dongle_left, coder->dongle_right, coder->simulator);
		coder_debug(coder);
		coder_refactor(coder);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_simulator	*simulation;
	int			i;

	simulation = (t_simulator *)arg;
	while (!should_stop(simulation))
	{
		i = 0;
		while (i < simulation->nb_coder)
		{
			if (get_current_time() >= simulation->coder[i].last_compilation + simulation->time_to_burnout)
			{
				print_log(&simulation->coder[i], "burned out");
				set_stop_flag(simulation);
				return (NULL);
			}
			i++;
		}
		usleep(300);
	}
	return (NULL);
}

static void	run_threads(t_simulator *simulation)
{
	int	i;

	pthread_create(&simulation->monitor_thread, NULL,
		monitor_routine, simulation);
	i = 0;
	while (i < simulation->nb_coder)
	{
		pthread_create(&simulation->coder[i].coder_thread, NULL,
			coder_routine, &simulation->coder[i]);
		i++;
	}
	i = 0;
	while (i < simulation->nb_coder)
	{
		pthread_join(simulation->coder[i].coder_thread, NULL);
		i++;
	}
	pthread_join(simulation->monitor_thread, NULL);
}

int	main(int argc, char *argv[])
{
	t_simulator	simulation;

	if (!parse_arguments(argc, argv, &simulation))
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	if (!init_simulation(&simulation))
	{
		fprintf(stderr, "Error: simulation initialization failed\n");
		return (1);
	}
	run_threads(&simulation);
	clean_simulation(&simulation);
	return (0);
}

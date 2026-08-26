/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 12:58:13 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/26 14:36:55 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (!should_stop(coder->simulator))
	{
		pthread_mutex_lock(&coder->lock);
		if (coder->nb_compilation >= coder->simulator->nb_compilation_required)
		{
			pthread_mutex_unlock(&coder->lock);
			break ;
		}
		pthread_mutex_unlock(&coder->lock);
		aquire_dongles(coder);
		coder_compile(coder);
		release_dongles(coder->dongle_left, coder->dongle_right,
			coder->simulator);
		coder_debug(coder);
		coder_refactor(coder);
	}
	return (NULL);
}

static int	check_coder(t_simulator *sim, t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	if (coder->nb_compilation >= sim->nb_compilation_required)
	{
		pthread_mutex_unlock(&coder->lock);
		return (1);
	}
	if (get_current_time() >= coder->last_compilation + sim->time_to_burnout)
	{
		pthread_mutex_unlock(&coder->lock);
		print_log(coder, "burned out");
		set_stop_flag(sim);
		return (-1);
	}
	pthread_mutex_unlock(&coder->lock);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_simulator	*sim;
	int			i;
	int			finished;
	int			res;

	sim = (t_simulator *)arg;
	while (!should_stop(sim))
	{
		i = 0;
		finished = 0;
		while (i < sim->nb_coder)
		{
			res = check_coder(sim, &sim->coder[i]);
			if (res == -1)
				return (NULL);
			finished += res;
			i++;
		}
		if (finished == sim->nb_coder)
			return (set_stop_flag(sim), NULL);
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

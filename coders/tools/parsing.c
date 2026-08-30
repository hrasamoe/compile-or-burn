/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 13:58:19 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/30 08:16:02 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/codexion.h"

static int	is_number(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static const char	*get_arg_name(int i)
{
	if (i == 1)
		return ("number_of_coders");
	if (i == 2)
		return ("time_to_burnout");
	if (i == 3)
		return ("time_to_compile");
	if (i == 4)
		return ("time_to_debug");
	if (i == 5)
		return ("time_to_refactor");
	if (i == 6)
		return ("number_of_compiles_required");
	return ("dongle_cooldown");
}

static void	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n\n");
	fprintf(stderr, "  the 7 numeric arguments must all be strictly "
		"positive integers.\n");
	fprintf(stderr, "  scheduler must be exactly \"fifo\" or \"edf\".\n\n");
	fprintf(stderr, "Example:\n  ./codexion 4 800 200 400 100 5 50 edf\n");
}

static int	validate_arguments(int argc, char **argv)
{
	int	i;

	if (argc != 9)
	{
		fprintf(stderr, "Error: expected 8 arguments, got %d\n",
			argc - 1);
		return (0);
	}
	i = 0;
	while (++i <= 7)
	{
		if (!is_number(argv[i]) || atoi(argv[i]) <= 0)
		{
			fprintf(stderr, "Error: %s (arg %d) must be a positive "
				"int, got \"%s\"\n", get_arg_name(i), i, argv[i]);
			return (0);
		}
	}
	if (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf"))
	{
		fprintf(stderr, "Error: scheduler (arg 8) must be \"fifo\" "
			"or \"edf\", got \"%s\"\n", argv[8]);
		return (0);
	}
	return (1);
}

int	parse_arguments(int argc, char **argv, t_simulator *sim)
{
	if (!validate_arguments(argc, argv))
	{
		print_usage();
		return (0);
	}
	sim->nb_coder = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refrator = atoi(argv[5]);
	sim->nb_compilation_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		sim->schedule_type = FIFO;
	else
		sim->schedule_type = EDF;
	return (1);
}

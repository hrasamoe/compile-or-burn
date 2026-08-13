/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 14:07:23 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 15:04:33 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>

typedef enum e_schedule
{
	FIFO,
	EDF
}	t_schedule;

typedef enum e_coder_state
{
	THINKING,
	DEBUGGING,
	COMPILING,
	BURNED_OUT,
	REFACTORING,
	WAITING_DONGLES
}	t_coder_state;

typedef struct s_simulator		t_simulator;
typedef struct s_heap			t_heap;
typedef struct s_coder			t_coder;
typedef struct s_dongle			t_dongle;
typedef struct s_request		t_request;

struct s_dongle
{
	int				id;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	int				held_by;
	int				is_available;
	long			unvailable_until;
};

struct s_request
{
	long			deadline;
	int				coder_id;
	long			arrival_time;
};

struct s_heap
{
	int				size;
	int				capacity;
	t_request		*request_array;
	pthread_mutex_t	lock;
	t_schedule		schedule_type;
};

struct s_coder
{
	int				id;
	pthread_mutex_t	lock;
	t_coder_state	state;
	long			deadline;
	t_simulator		*simulator;
	t_dongle		*dongle_left;
	t_dongle		*dongle_right;
	int				nb_compilation;
	long			last_compilation;
};

struct s_simulator
{
	int				stop;
	t_coder			*coder;
	t_dongle		*dongles;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	stop_lock;
	pthread_mutex_t	heap_lock;
	int				nb_coder;
	long			start_time;
	long			time_to_debug;
	t_heap			*request_heap;
	t_schedule		schedule_type;
	long			time_to_compile;
	long			dongle_cooldown;
	long			time_to_burnout;
	long			time_to_refrator;
	int				nb_compilation_required;
};

long long	get_current_time(void);
void		free_dongle_on_error(t_dongle *dongles, int i);

#endif

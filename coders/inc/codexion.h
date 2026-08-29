/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 14:07:23 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/29 22:04:40 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

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

typedef struct s_simulator	t_simulator;
typedef struct s_heap		t_heap;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;
typedef struct s_request	t_request;

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
	pthread_t		coder_thread;
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
	pthread_t		monitor_thread;
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

void		print_start_msg(void);
long long	get_current_time(void);
t_request	*heap_pop(t_heap *heap);
t_request	*peek_heap(t_heap *heap);
void		coder_debug(t_coder *coder);
void		coder_compile(t_coder *coder);
void		aquire_dongles(t_coder *coder);
void		coder_refactor(t_coder *coder);
void		print_err_msg(const char *msg);
void		heap_shift(t_heap *heap, int i);
void		print_finish_msg(t_simulator *sim);
int			should_stop(t_simulator *simulation);
void		set_stop_flag(t_simulator *simulation);
int			init_simulation(t_simulator *simulation);
void		clean_simulation(t_simulator *simulation);
void		free_dongle_on_error(t_dongle *dongles, int i);
void		print_log(t_coder *coder, const char *action);
int			push_heap(t_heap *heap, t_request new_request);
void		precise_sleep(t_simulator *simulation, long duration);
int			parse_arguments(int argc, char **argv, t_simulator *sim);
int			are_dongles_ready(t_dongle *dongle_left, t_dongle *dongle_right);
int			compare_heap(t_request a, t_request b, t_schedule schedule_type);
void		take_dongles(t_coder *coder,
				t_dongle *dongle_left,
				t_dongle *dongle_right);
void		release_dongles(t_dongle *dongle_left,
				t_dongle *dongle_right,
				t_simulator *simulator);

#endif

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasamoe <hrasamoe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/07 14:07:23 by hrasamoe          #+#    #+#             */
/*   Updated: 2026/08/13 10:50:31 by hrasamoe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

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

#endif

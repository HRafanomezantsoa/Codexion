/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/19 23:02:54 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 13:14:14 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <limits.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef enum e_coder_state
{
	coder_waiting,
	coder_compile,
	coder_debuging,
	coder_refactoring
}	t_coder_state;

typedef struct s_args
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
}	t_args;

typedef struct s_request
{
	int			coder_id;
	long long	timestamp;
	long long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*data;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	int				available;
	long long		cooldown_end;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_heap			heap;
}	t_dongle;

typedef struct s_sim	t_sim;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_coder_state	state;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	long long		last_compile_start;
	long long		deadline;
	int				compiles_done;
	struct s_sim	*sim;
}	t_coder;

struct s_sim
{
	t_args			*args;
	t_dongle		*dongles;
	t_coder			*coders;
	int				running;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	state_mutex;
	long long		start_time;
};

int			is_number(char *str);
int			parse_int(char *str, int *result);
int			valid_args(char **av, t_args *args);
int			init_simulation(t_sim *sim, t_args *args);
int			heap_init(t_heap *heap, int capacity);
long long	get_time_ms(void);
void		cleanup_simulation(t_sim *sim);
void		log_state(t_sim *sim, int coder_id, const char *msg);
void		heap_free(t_heap *heap);
int			compare_requests(t_request a, t_request b, t_scheduler sched);
void		heap_push(t_heap *heap, t_request req, t_scheduler sched);
t_request	heap_pop(t_heap *heap, t_scheduler sched);
t_request	heap_keep(t_heap *heap);
int			dongle_take(t_dongle *dongle, t_coder *coder, t_sim *sim);
void		dongle_release(t_dongle *dongle, t_sim *sim);
void		*coder_routine(void *arg);
int			check_running(t_sim *sim);
int			do_debug_and_refactor(t_coder *coder, t_sim *sim);
void		*monitor_routine(void *arg);
#endif

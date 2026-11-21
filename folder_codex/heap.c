/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: njrafano <njrafano@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/21 09:36:40 by njrafano          #+#    #+#             */
/*   Updated: 2026/11/21 12:48:10 by njrafano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_free(t_heap *heap)
{
	free(heap->data);
	heap->data = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

int	compare_requests(t_request a, t_request b, t_scheduler sched)
{
	if (sched == FIFO)
	{
		if (a.timestamp < b.timestamp)
			return (-1);
		else if (a.timestamp > b.timestamp)
			return (1);
	}
	else if (sched == EDF)
	{
		if (a.deadline < b.deadline)
			return (-1);
		else if (a.deadline > b.deadline)
			return (1);
	}
	if (a.coder_id < b.coder_id)
		return (-1);
	else if (a.coder_id > b.coder_id)
		return (1);
	return (0);
}

void	heap_push(t_heap *heap, t_request req, t_scheduler sched)
{
	int			i;
	int			parent;
	t_request	tmp;

	if (heap->size >= heap->capacity)
		return ;
	heap->data[heap->size] = req;
	heap->size++;
	i = (heap->size) - 1;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (compare_requests(heap->data[i], heap->data[parent], sched) < 0)
		{
			tmp = heap->data[i];
			heap->data[i] = heap->data[parent];
			heap->data[parent] = tmp;
			i = parent;
		}
		else
			break ;
	}
}

static void	heapify_down(t_heap *heap, int i, t_scheduler sched)
{
	int			left;
	int			right;
	int			smallest;
	t_request	tmp;

	while (1)
	{
		left = (2 * i) + 1;
		right = (2 * i) + 2;
		smallest = i;
		if (left < heap->size && (
				compare_requests(
					heap->data[left], heap->data[smallest], sched) < 0))
			smallest = left;
		if (right < heap->size && (
				compare_requests(
					heap->data[right], heap->data[smallest], sched) < 0))
			smallest = right;
		if (smallest == i)
			break ;
		tmp = heap->data[smallest];
		heap->data[smallest] = heap->data[i];
		heap->data[i] = tmp;
		i = smallest;
	}
}

t_request	heap_pop(t_heap *heap, t_scheduler sched)
{
	t_request	result;
	t_request	empty;

	if (heap->size == 0)
	{
		memset(&empty, 0, sizeof(empty));
		return (empty);
	}
	result = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	heap->size--;
	heapify_down(heap, 0, sched);
	return (result);
}

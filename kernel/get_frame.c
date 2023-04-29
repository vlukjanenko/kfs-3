/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_frame.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majosue <majosue@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 10:50:09 by majosue           #+#    #+#             */
/*   Updated: 2023/04/15 12:07:22 by majosue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memory.h"
#include "stdint.h"
#include "baselib.h"
#include "stddef.h"

static unsigned char	*bitmask;
static uint32_t			bm_size;

static void frame_allocator_init()
{
	bitmask = get_bitmask();
	bm_size = get_bitmask_size();
}

int frame_status(uint32_t addr)
{
	return (bitmask[addr / 0x1000 / 8] & \
		(1 << addr / 0x1000 % 8));
}

void free_frame(void* addr)
{
	bitmask[(uint32_t)addr / 0x1000 / 8] = bitmask[(uint32_t)addr / 0x1000 / 8] & \
		~((bitmask[(uint32_t)addr / 0x1000 / 8] & \
			(1 << (uint32_t)addr / 0x1000 % 8)));
}

static void *mark_frames_occupied(uint32_t i, int j, uint32_t frames)
{
	void* ret_value = (void *)(0x1000 * (i * 8 + j));
	uint32_t nbr = 0;

	for ( ; nbr < frames; i++) {
		for (; j < 8 && nbr < frames; j++) {
			bitmask[i] = bitmask[i] | (1 << j);
			nbr++;
		}
		j = 0;
	}
	return (ret_value);
}

static void *__get_frames(uint32_t *i, int *j, uint32_t frames)
{
	uint32_t nbr = 0;
	uint32_t i_begin = *i;
	uint32_t j_begin = *j;

	for (; *i < bm_size; (*i)++) {
		for (; *j < 8; (*j)++) {
			if (!(bitmask[*i] & (1 << *j)))
				nbr++;
			else
				return (NULL);
			if (nbr == frames)
				return mark_frames_occupied(i_begin, j_begin, frames);
		}
		*j = 0;
	}
	return (NULL);
}

static void addr_to_idxs(uint32_t pfn, uint32_t *i, uint32_t *j)
{
	*i = pfn / 8;
	*j = pfn % 8;
}

static uint32_t idxs_to_pfn(uint32_t i, uint32_t j)
{
	return (i * 8 + j);
}

void *get_frames(uint32_t start_frame, uint32_t end_frame, uint32_t frames)
{
	void	*ret_value = NULL;
	uint32_t i;
	uint32_t j;

	if (!bitmask) {
		frame_allocator_init();
	}

	for (; i < bm_size && !ret_value; i++) {
		if (bitmask[i] == 0xFF)
			continue;
		for (; j < 8 && !ret_value; j++) { // тут проверка чтоб не вылезли за максимальный адрес?
			if (!(bitmask[i] & (1 << j))) {
				ret_value = __get_frames(&i, &j, frames);
			}
		}
		j = 0;
	}
	return (ret_value);
}

void *get_frame(uint32_t start_frame, uint32_t end_frame)
{
	return (get_frames(start_frame, end_frame, 1));
}

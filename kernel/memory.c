/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majosue <majosue@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/30 00:05:27 by majosue           #+#    #+#             */
/*   Updated: 2023/04/30 00:17:46 by majosue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "memory.h"
#include "stdint.h"
#include "baselib.h"
#include "stddef.h"

#define PHISYCAL_EOC ((uint32_t)&end_of_code - PAGE_OFFSET)

static unsigned char	*bitmask = &end_of_code;
static uint32_t			bm_size;
static uint32_t			*tables;
static void				*heap_start;
static void				*heap_end; // следующий за последним валидным адресом

unsigned char*	get_bitmask()
{
	return (bitmask);
}

uint32_t	get_bitmask_size()
{
	return (bm_size);
}

void	*get_heap_start()
{
	return (heap_start);
}

void	*get_heap_end()
{
	return (heap_end);
}

uint32_t	align(uint32_t addr)
{
	return (((addr + (PAGE_SIZE - 1)) / PAGE_SIZE) * PAGE_SIZE);
}

/*
	первые 512М физической  0x0 - 0x20000000 мапим к 0xC000000 - 0xE0000000 виртуальной
*/
static void	intital_map()
{
	uint32_t	*pde = (uint32_t *)0xFFFFF000;
	uint32_t	frames = 0;

	for (int i = 768; i < 896; i++, tables += 1024) {
		for (int j = 0; j < 1024; j++, frames += PAGE_SIZE) {
			tables[j] = frames | 3;
		}
		pde[i] = ((uint32_t)tables - PAGE_OFFSET) | 3; // в pde физические адреса таблиц!
	}
	refresh_map();
}

void memory_init()
{
	/*
		На начало у нас смаплена 1 страница для загрузки.
		Нам надо смапить до конца кучи 0xE0000000 всю физическую память
		тогда доступ к физической будет
		физический адрес = виртуальлный - PAGE_OFFSET
		это для первых 512М
	*/

	uint32_t frames_in_use;

	// размер доступной физической памяти из загрузчика - align(MAX_ADDR)
	printf("Size of ph. memory (page aligned) = %x\n", align(MAX_ADDR));
	// размер массива для описания памяти - bm_size
	bm_size = align(MAX_ADDR) / PAGE_SIZE / 8;
	printf("Size of bit bitmask array %u bytes\n", bm_size);
	// занято страниц кодом и битовой маской - frames_in_use
	frames_in_use = PHISYCAL_EOC / PAGE_SIZE + align(bm_size) / PAGE_SIZE;
	// начало таблиц для трансляии 512M (128 таблиц)
	tables = (uint32_t *)(frames_in_use * PAGE_SIZE + PAGE_OFFSET);
	// тут начнется куча
	heap_start = (void *)((frames_in_use + 128) * PAGE_SIZE + PAGE_OFFSET);
	heap_end = (void *)0xE0000000; // тут куча закончится (sbrk будет это менять ?)
	intital_map();
	// помечаем занятые фреймы
	bzero(bitmask, bm_size);
	for (uint32_t i = 0; i < frames_in_use + 128 ; i++) {
		bitmask[i / 8] = bitmask[i / 8] | (1 << (i % 8));
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GDT.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majosue <majosue@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/01 18:58:31 by majosue           #+#    #+#             */
/*   Updated: 2023/01/26 23:09:47 by majosue          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GDT_H
# define GDT_H

# include "stdint.h"

struct __attribute__((packed)) GDTR
{
	uint16_t limit;
	uint32_t base;
};

extern uint64_t		g_table[6];
extern struct GDTR	gdtr;

void	init_gdt();

#endif
/*
 * OMAP4 Secure Monitor Calls wrapper implementations
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *	Carlos Leija
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <config.h>
#include <asm/arch/cpu.h>
#include <asm/arch/omap4430.h>
#include <asm/arch/smc.h>

/**
 * omap_smc_rom() - This function is the entry point to api's exposed by
 *		    ROM code
 * Prototype:
 * unsigned int omap_smc_rom(unsigned int rom_svc,
 *		unsigned int arg);
 * @rom_svc: Application ID of ROM api (input in r0 will move to r12)
 * @arg: Argument passed to ROM api (input in r1 will move to r0)
 */
.globl omap_smc_rom
omap_smc_rom:
	push {r4-r11,lr}
	mov	r12,	r0
	mov	r0,	r1
	dsb
	isb
	dmb
	smc	#0
	pop {r4-r11,pc}

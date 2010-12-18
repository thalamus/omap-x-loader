/*
 *  armboot - Startup Code for OMAP4430/ARM Cortex CPU-core
 *
 *  Copyright (c) 2004	Texas Instruments <r-woodruff2@ti.com>
 *
 *  Copyright (c) 2001	Marius Groger <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Zupke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <gj@denx.de>
 *  Copyright (c) 2003	Richard Woodruff <r-woodruff2@ti.com>
 *  Copyright (c) 2003	Kshitij <kshitij@ti.com>
 *  Copyright (c) 2006	Syed Mohammed Khasim <x0khasim@ti.com>
 *  Copyright (c) 2009	Texas Instruments
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#if !defined(CONFIG_INTEGRATOR) && ! defined(CONFIG_ARCH_CINTEGRATOR)
#include <asm/arch/cpu.h>
#endif
#define CONFIG_SKIP_RELOCATE_XLOADER
.globl _start
_start:
	b	reset
 	ldr	pc, _hang
	ldr	pc, _hang
	ldr	pc, _hang
	ldr	pc, _hang
	ldr	pc, _hang
	ldr	pc, _hang
	ldr	pc, _hang

_hang:
 	.word do_hang
	
	.word 0x12345678
	.word 0x12345678
	.word 0x12345678
	.word 0x12345678
	.word 0x12345678
	.word 0x12345678
	.word 0x12345678 /* now 16*4=64 */

.global _end_vect
_end_vect:

	.balignl 16,0xdeadbeef
/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

/*
 * the actual reset code
 */

reset:
	/* Store the boot reason/device in scratchpad */
	ldr     r4, scratchpad
	ldr     r5, [r0, #0x8]          /* r5 <- value of boot device */
	bic	r5, r5, #0xFFFFFF00	@ clear bits other than first byte*/
	str     r5, [r4]                /* store r5 in scratchpad reg */

	cmp	r5, #0x40
	bgt	skip_type_check

	/* Store the boot mode (raw/FAT) in scratchpad */
	ldr	r5, [r0, #0x4]		/* get the device descriptor ptr */
	ldr	r5, [r5, #0x18]		/* get the pDeviceData ptr */
	ldr	r5, [r5, #0x8]		/* get the boot mode */
	str	r5, [r4, #0x4]		/* store boot mode in scratchpad+4 */

skip_type_check:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0

       /* Copy vectors to mask ROM indirect addr */
	adr	r0, _start		/* r0 <- current position of code   */
        add     r0, r0, #4				/* skip reset vector */
	mov	r2, #64			/* r2 <- size to copy  */
	add	r2, r0, r2		/* r2 <- source end address	    */
	mov	r1, #SRAM_OFFSET0	  /* build vect addr */
	mov	r3, #SRAM_OFFSET1
	add	r1, r1, r3
	mov	r3, #SRAM_OFFSET2
	add	r1, r1, r3
next:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end address [r2]    */
	bne	next			/* loop until equal */
	/* the mask ROM code should have PLL and others stable */
	bl  cpu_init_crit

#ifndef CONFIG_SKIP_RELOCATE_XLOADER
relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp	r0, r1			/* don't reloc during debug	    */
	beq	stack_setup

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot	    */
	add	r2, r0, r2		/* r2 <- source end address	    */

copy_loop:                              /* copy 32 bytes at a time          */
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop
#endif

	/* Set up the stack						    */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	and	sp, sp, #~7		/* 8 byte alinged for (ldr/str)d    */

	/* Clear BSS (if any).  Is below tx (watch load addr - need space)  */
clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov 	r2, #0x00000000		/* clear value                      */
clbss_l:
	str	r2, [r0]		/* clear BSS location               */
	cmp	r0, r1			/* are we at the end yet            */
	add	r0, r0, #4		/* increment clear index pointer    */
	bne	clbss_l                 /* keep clearing till at end        */

	ldr	pc, _start_armboot	/* jump to C code                   */

_start_armboot: .word start_armboot


/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */
cpu_init_crit:
	/*
	 * Invalidate L1 I/D
	 */
        mov	r0, #0                 /* set up for MCR */
        mcr	p15, 0, r0, c8, c7, 0  /* invalidate TLBs */
        mcr	p15, 0, r0, c7, c5, 0  /* invalidate icache */
	mcr	p15, 0, r0, c7, c5, 6  /* invalidate BP array */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0

	/*
	 * Jump to board specific initialization... The Mask ROM will
         * have already initialized basic memory.  Go here to bump up
         * clock rate and handle wake up conditions.
	 */
	mov	ip, lr		/* persevere link reg across call */
	bl	lowlevel_init	/* go setup pll,mux,memory */
	mov	lr, ip		/* restore link */
	mov	pc, lr		/* back to my caller */


/*
 * exception handler
 */
 	.align  5
do_hang:
	ldr	sp, _TEXT_BASE		/* use 32 words abort stack */
   	bl	hang			/* hang and never return */

scratchpad:
    .word 0x4A326000

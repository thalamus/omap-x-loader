/*
 * omap4430.h
 *
 * Copyright(c) 2010 Texas Instruments.   All rights reserved.
 *
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
 * Syed Mohammed Khasim <x0khasim@ti.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Texas Instruments nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _OMAP4430_SYS_H_
#define _OMAP4430_SYS_H_

#include <asm/arch/sizes.h>

/*
 * 4430 specific Section
 */

/* Stuff on L3 Interconnect */
#define SMX_APE_BASE			0x68000000

/* L3 Firewall */
#define A_REQINFOPERM0		(SMX_APE_BASE + 0x05048)
#define A_READPERM0		(SMX_APE_BASE + 0x05050)
#define A_WRITEPERM0		(SMX_APE_BASE + 0x05058)

/* GPMC */
#define OMAP44XX_GPMC_BASE		(0x50000000)

/* DMM */
#define OMAP44XX_DMM_BASE		0x4E000000

/* SMS */
#define OMAP44XX_SMS_BASE               0x6C000000

/* SDRC */
#define OMAP44XX_SDRC_BASE              0x6D000000


/*
 * L4 Peripherals - L4 Wakeup and L4 Core now
 */
#define OMAP44XX_CORE_L4_IO_BASE	0x4A000000

#define OMAP44XX_WAKEUP_L4_IO_BASE	0x4A300000

#define OMAP44XX_L4_PER			0x48000000

#define OMAP44XX_L4_IO_BASE		OMAP44XX_CORE_L4_IO_BASE

/* CONTROL */
#define OMAP44XX_CTRL_GEN_BASE		(OMAP44XX_L4_IO_BASE+0x2000)
#define OMAP44XX_CTRL_PADCONF_CORE_BASE	0x4a100000

/* PL310 */
#define OMAP44XX_PL310_BASE		0x48242000

/* TAP information  dont know for 3430*/
#define OMAP44XX_TAP_BASE	(0x49000000) /*giving some junk for virtio */

/* UART */
#define OMAP44XX_UART1			(OMAP44XX_L4_PER+0x6a000)
#define OMAP44XX_UART2			(OMAP44XX_L4_PER+0x6c000)
#define OMAP44XX_UART3			(OMAP44XX_L4_PER+0x20000)

/* General Purpose Timers */
#define OMAP44XX_GPT1			0x48318000
#define OMAP44XX_GPT2			0x48032000
#define OMAP44XX_GPT3			0x48034000
#define OMAP44XX_GPT4			0x48036000
#define OMAP44XX_GPT5			0x40138000
#define OMAP44XX_GPT6			0x4013A000
#define OMAP44XX_GPT7			0x4013C000
#define OMAP44XX_GPT8			0x4013E000
#define OMAP44XX_GPT9			0x48040000
#define OMAP44XX_GPT10			0x48086000
#define OMAP44XX_GPT11			0x48088000
#define OMAP44XX_GPT12			0x48304000

/* WatchDog Timers (1 secure, 3 GP) */
#define WD1_BASE			(0x4A322000)
#define WD2_BASE			(0x4A314000)
#define WD3_BASE			(0x40130000)

/* 32KTIMER */
#define SYNC_32KTIMER_BASE		(0x48320000)
#define S32K_CR				(SYNC_32KTIMER_BASE+0x10)

/*
 * SDP4430 specific Section
 */

/*
 *  The 443x's chip selects are programmable.  The mask ROM
 *  does configure CS0 to 0x08000000 before dispatch.  So, if
 *  you want your code to live below that address, you have to
 *  be prepared to jump though hoops, to reset the base address.
 *  Same as in SDP4430
 */
#ifdef CONFIG_OMAP44XX
/* base address for indirect vectors (internal boot mode) */
#define SRAM_OFFSET0                    0x40000000
#define SRAM_OFFSET1                    0x00300000
#define SRAM_OFFSET2                    0x0000D000
#define SRAM_OFFSET3                    0x00000800
#define SRAM_VECT_CODE                  (SRAM_OFFSET0|SRAM_OFFSET1|SRAM_OFFSET2|SRAM_OFFSET3)
#define LOW_LEVEL_SRAM_STACK            0x4030DFFC
#endif

#if defined(CONFIG_4430SDP)
/* FPGA on Debug board.*/
# define ETH_CONTROL_REG			(DEBUG_BASE+0x30b)
# define LAN_RESET_REGISTER		(DEBUG_BASE+0x1c)

# define DIP_SWITCH_INPUT_REG2		(DEBUG_BASE+0x60)
# define LED_REGISTER			(DEBUG_BASE+0x40)
# define FPGA_REV_REGISTER		(DEBUG_BASE+0x10)
# define EEPROM_MAIN_BRD			(DEBUG_BASE+0x10000+0x1800)
# define EEPROM_CONN_BRD			(DEBUG_BASE+0x10000+0x1900)
# define EEPROM_UI_BRD			(DEBUG_BASE+0x10000+0x1A00)
# define EEPROM_MCAM_BRD			(DEBUG_BASE+0x10000+0x1B00)
# define ENHANCED_UI_EE_NAME		"750-2075"
#endif

#endif  /* _OMAP4430_SYS_H_ */

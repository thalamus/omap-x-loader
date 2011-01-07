/*
 * (C) Copyright 2006
 * Texas Instruments <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
 *
 * X-Loader Configuation settings for the TI OMAP SDP3430 board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/* serial printf facility takes about 3.5K */
#define CFG_PRINTF
//#undef CFG_PRINTF

/*
 * High Level Configuration Options
 */
#define CONFIG_ARMCORTEXA9       1    /* This is an ARM V7 CPU core */
#define CONFIG_OMAP              1    /* in a TI OMAP core */
#define CONFIG_OMAP44XX          1    /* which is a 34XX */
#define CONFIG_OMAP4430          1    /* which is in a 3430 */
#define CONFIG_4430SDP           1    /* working with SDP */

#define CORE_190MHZ		1
/* Enable the below macro if MMC boot support is required */
#define CONFIG_MMC               1
#if defined(CONFIG_MMC)
	/* To Enable MMC boot for OMAP4430 SDP */
	//#define CONFIG_MMC1		1
	/* To Enable EMMC boot for OMAP4430 SDP */
	#define CONFIG_MMC2		1
	#define CFG_CMD_MMC		1
	#define CFG_CMD_FAT		1
#endif

#include <asm/arch/cpu.h>        /* get chip and board defs */

/* uncomment it if you need timer based udelay(). it takes about 250 bytes */
//#define CFG_UDELAY

/* Clock Defines */
#define V_OSCK                   19200000  /* Clock output from T2 */
#define V_SCLK                   V_OSCK

/* Memory type */
#define CFG_4430SDRAM_DDR        1

#ifdef CFG_PRINTF

#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE     (-4)
#define CFG_NS16550_CLK          (48000000)
#define CFG_NS16550_COM3         OMAP44XX_UART3

/*
 * select serial console configuration
 */
#define CONFIG_SERIAL1           1    /* UART1 on 3430SDP */
#define CONFIG_CONS_INDEX        3

#define CONFIG_BAUDRATE          115200
#define CFG_PBSIZE               256

#endif /* CFG_PRINTF */

/*
 * Miscellaneous configurable options
 */
#define CFG_LOADADDR             0x80e80000

#undef	CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */
#define CFG_ENV_SIZE		0x100
/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE         (128*1024) /* regular stack */

#endif /* __CONFIG_H */


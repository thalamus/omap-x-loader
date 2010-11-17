/*
 * (C) Copyright 2004-2009 Texas Insturments
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * CPU specific code
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mem.h>

/* See also ARM Ref. Man. */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */
#define C1_WB		(1<<3)		/* merging write buffer on/off */
#define C1_BIG_ENDIAN	(1<<7)		/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13) /* location of vectors: low/high addresses */
#define RESERVED_1	(0xf << 3)	/* must be 111b for R/W */

int cpu_init (void)
{
	return 0;
}

unsigned int cortex_a9_rev(void)
{

	unsigned int i;

	/* turn off I/D-cache */
	asm ("mrc p15, 0, %0, c0, c0, 0" : "=r" (i));

	return i;
}

unsigned int omap_revision(void)
{
	unsigned int rev = cortex_a9_rev();

	if (__raw_readl(0x4a002204) == 0x3b95c02f)
		return OMAP4430_ES2_1;

	switch(rev) {
	case 0x410FC091:
		return OMAP4430_ES1_0;
	case 0x411FC092:
		return OMAP4430_ES2_0;
	default:
		return OMAP4430_SILICON_ID_INVALID;
	}
}

unsigned int get_boot_mode(void)
{
	/* retrieve the boot mode stored in scratchpad */
	return (*(volatile unsigned int *)(0x4A326004)) & 0xf;
}

unsigned int get_boot_device(void)
{
	/* retrieve the boot device stored in scratchpad */
	return (*(volatile unsigned int *)(0x4A326000)) & 0xff;
}
unsigned int raw_boot(void)
{
	if (get_boot_mode() == 1)
		return 1;
	else
		return 0;
}

unsigned int fat_boot(void)
{
	if (get_boot_mode() == 2)
		return 1;
	else
		return 0;
}

#if defined(CONFIG_MPU_600) || defined(CONFIG_MPU_1000)
static void scale_vcores(void)
{
	unsigned int rev = omap_revision();
	/* For VC bypass only VCOREx_CGF_FORCE  is necessary and
	 * VCOREx_CFG_VOLTAGE  changes can be discarded
	 */
	/* PRM_VC_CFG_I2C_MODE */
	*(volatile int*)(0x4A307BA8) = 0x0;
	/* PRM_VC_CFG_I2C_CLK */
	*(volatile int*)(0x4A307BAC) = 0x6026;

	/* set VCORE1 force VSEL */
	/* PRM_VC_VAL_BYPASS) */
        if(rev == OMAP4430_ES1_0)
		*(volatile int*)(0x4A307BA0) = 0x3B5512;
	else if (rev == OMAP4430_ES2_0)
		*(volatile int*)(0x4A307BA0) = 0x3A5512;
	else if (rev == OMAP4430_ES2_1)
		*(volatile int*)(0x4A307BA0) = 0x3A5512;

	*(volatile int*)(0x4A307BA0) |= 0x1000000;
	while((*(volatile int*)(0x4A307BA0)) & 0x1000000);

	/* PRM_IRQSTATUS_MPU */
	*(volatile int*)(0x4A306010) = *(volatile int*)(0x4A306010);


	/* FIXME: set VCORE2 force VSEL, Check the reset value */
	/* PRM_VC_VAL_BYPASS) */
        if(rev == OMAP4430_ES1_0)
		*(volatile int*)(0x4A307BA0) = 0x315B12;
	else
		*(volatile int*)(0x4A307BA0) = 0x295B12;
	*(volatile int*)(0x4A307BA0) |= 0x1000000;
	while((*(volatile int*)(0x4A307BA0)) & 0x1000000);

	/* PRM_IRQSTATUS_MPU */
	*(volatile int*)(0x4A306010) = *(volatile int*)(0x4A306010);

	/*/set VCORE3 force VSEL */
	/* PRM_VC_VAL_BYPASS */
        if(rev == OMAP4430_ES1_0)
		*(volatile int*)(0x4A307BA0) = 0x316112;
	else if (rev == OMAP4430_ES2_0)
		*(volatile int*)(0x4A307BA0) = 0x296112;
	else if (rev == OMAP4430_ES2_1)
		*(volatile int*)(0x4A307BA0) = 0x2A6112;

	*(volatile int*)(0x4A307BA0) |= 0x1000000;
	while((*(volatile int*)(0x4A307BA0)) & 0x1000000);

	/* PRM_IRQSTATUS_MPU */
	*(volatile int*)(0x4A306010) = *(volatile int*)(0x4A306010);

}
#endif



/**********************************************************
 * Routine: s_init
 * Description: Does early system init of muxing and clocks.
 * - Called path is with SRAM stack.
 **********************************************************/
void s_init(void)
{
	set_muxconf_regs();
	spin_delay(100);

	/* Set VCORE1 = 1.3 V, VCORE2 = VCORE3 = 1.21V */
#if defined(CONFIG_MPU_600) || defined(CONFIG_MPU_1000)
	scale_vcores();
#endif	

	prcm_init();
	ddr_init();


}

/******************************************************
 * Routine: wait_for_command_complete
 * Description: Wait for posting to finish on watchdog
 ******************************************************/
void wait_for_command_complete(unsigned int wd_base)
{
	int pending = 1;
	do {
		pending = __raw_readl(wd_base + WWPS);
	} while (pending);
}

int nand_init(void)
{
	return 1;
}

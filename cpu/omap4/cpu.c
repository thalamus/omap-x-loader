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
#include <asm/arch/bits.h>
#include <asm/arch/cpu.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/smc.h>

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

#define PL310_POR	5

int cpu_init (void)
{
	unsigned int es_revision;

	es_revision = omap_revision();

	if (es_revision > OMAP4430_ES1_0 && get_device_type() != GP_DEVICE) {
		/* Set PL310 Prefetch Offset Register w/PPA svc*/
		omap_smc_ppa(PPA_SERVICE_PL310_POR, 0, 0x7, 1, PL310_POR);
		/* Enable L2 data prefetch */
		omap_smc_rom(ROM_SERVICE_PL310_AUXCR_SVC,
			__raw_readl(OMAP44XX_PL310_AUX_CTRL) | 0x10000000);
	} else if (es_revision > OMAP4430_ES2_1) {
		/* Set PL310 Prefetch Offset Register using ROM svc */
		omap_smc_rom(ROM_SERVICE_PL310_POR_SVC, PL310_POR);
		/* Enable L2 data prefetch */
		omap_smc_rom(ROM_SERVICE_PL310_AUXCR_SVC,
			__raw_readl(OMAP44XX_PL310_AUX_CTRL) | 0x10000000);
	}

	/* For ES2.2
	 * 1. If unit does not have SLDO trim, set override
	 * and force max multiplication factor to ensure
	 * proper SLDO voltage at low OPP's
	 * 2. Trim VDAC value for TV out as recomended to avoid
	 * potential instabilities at low OPP's
	 * 3.For all ESx.y trimmed and untrimmed units
	 * Override efuse with LPDDR P:16/N:16 and
	 * smart IO P:0/N:0 as per recomendation
	 */
	__raw_writel(0x00084000, SYSCTRL_PADCONF_CORE_EFUSE_2);
	if (es_revision == OMAP4430_ES2_2) {
		/*if MPU_VOLTAGE_CTRL is 0x0 unit is not trimmed*/
		if (!__raw_readl(IVA_LDOSRAM_VOLTAGE_CTRL)) {
			/* Set M factor to max (2.7) */
			__raw_writel(0x0401040f, IVA_LDOSRAM_VOLTAGE_CTRL);
			__raw_writel(0x0401040f, MPU_LDOSRAM_VOLTAGE_CTRL);
			__raw_writel(0x0401040f, CORE_LDOSRAM_VOLTAGE_CTRL);
			__raw_writel(0x000001c0, SYSCTRL_PADCONF_CORE_EFUSE_1);
		}
	}

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
	/*
	 * For some of the ES2/ES1 boards ID_CODE is not reliable:
	 * Also, ES1 and ES2 have different ARM revisions
	 * So use ARM revision for identification
	 */
	unsigned int rev = cortex_a9_rev();

	switch (rev) {
		case MIDR_CORTEX_A9_R0P1:
			return OMAP4430_ES1_0;
		case MIDR_CORTEX_A9_R1P2:
			rev = readl(CONTROL_ID_CODE);
			switch (rev) {
				case OMAP4_CONTROL_ID_CODE_ES2_3:
					return OMAP4430_ES2_3;
				case OMAP4_CONTROL_ID_CODE_ES2_2:
					return OMAP4430_ES2_2;
				case OMAP4_CONTROL_ID_CODE_ES2_1:
					return OMAP4430_ES2_1;
				case OMAP4_CONTROL_ID_CODE_ES2_0:
					return OMAP4430_ES2_0;
				default:
					return OMAP4430_ES2_0;
			}
		case MIDR_CORTEX_A9_R2P8:
			return OMAP4460_ES1_0;
		default:
			return OMAP4430_SILICON_ID_INVALID;
	}
}

u32 get_device_type(void)
{
	/*
	 * Retrieve the device type: GP/EMU/HS/TST stored in
	 * CONTROL_STATUS
	 */
	return (__raw_readl(CONTROL_STATUS) & DEVICE_MASK) >> 8;
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
	__raw_writel(0x0, 0x4A307BA8);
	/* PRM_VC_CFG_I2C_CLK */
	__raw_writel(0x6026, 0x4A307BAC);

	/* set VCORE1 force VSEL */
	/* PRM_VC_VAL_BYPASS) */
        if(rev == OMAP4430_ES1_0)
		__raw_writel(0x3B5512, 0x4A307BA0);
	else if (rev == OMAP4430_ES2_0)
		__raw_writel(0x3A5512, 0x4A307BA0);
	else if (rev >= OMAP4430_ES2_1)
		__raw_writel(0x3A5512, 0x4A307BA0);

	__raw_writel(__raw_readl(0x4A307BA0) | 0x1000000, 0x4A307BA0);
	while(__raw_readl(0x4A307BA0) & 0x1000000)
		;

	/* PRM_IRQSTATUS_MPU */
	__raw_writel(__raw_readl(0x4A306010), 0x4A306010);


	/* FIXME: set VCORE2 force VSEL, Check the reset value */
	/* PRM_VC_VAL_BYPASS) */
        if(rev == OMAP4430_ES1_0)
		__raw_writel(0x315B12, 0x4A307BA0);
	else
		__raw_writel(0x295B12, 0x4A307BA0);
	__raw_writel(__raw_readl(0x4A307BA0) | 0x1000000, 0x4A307BA0);
	while(__raw_readl(0x4A307BA0) & 0x1000000)
		;

	/* PRM_IRQSTATUS_MPU */
	__raw_writel(__raw_readl(0x4A306010), 0x4A306010);

	/*/set VCORE3 force VSEL */
	/* PRM_VC_VAL_BYPASS */
        if(rev == OMAP4430_ES1_0)
		__raw_writel(0x316112, 0x4A307BA0);
	else if (rev == OMAP4430_ES2_0)
		__raw_writel(0x296112, 0x4A307BA0);
	else if (rev >= OMAP4430_ES2_1)
		__raw_writel(0x2A6112, 0x4A307BA0);

	__raw_writel(__raw_readl(0x4A307BA0) | 0x1000000, 0x4A307BA0);

	while(__raw_readl(0x4A307BA0) & 0x1000000)
		;

	/* PRM_IRQSTATUS_MPU */
	__raw_writel(__raw_readl(0x4A306010), 0x4A306010);

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

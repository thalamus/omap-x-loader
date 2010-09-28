/*
 * (C) Copyright 2004-2009
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
 * Santosh Shilimkar <santosh.shilimkar@ti.com>
 * Aneesh V	<aneesh@ti.com>
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
#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mem.h>

#define CONFIG_OMAP4_SDC		1

#define MR0_ADDR			0
#define MR1_ADDR			1
#define MR2_ADDR			2
#define MR4_ADDR			4
#define MR10_ADDR			10
#define MR16_ADDR			16
#define REF_EN				0x40000000
/* defines for MR1 */
#define MR1_BL4				2
#define MR1_BL8				3
#define MR1_BL16			4

#define MR1_BT_SEQ			0
#define BT_INT				1

#define MR1_WC				0
#define MR1_NWC				1

#define MR1_NWR3			1
#define MR1_NWR4			2
#define MR1_NWR5			3
#define MR1_NWR6			4
#define MR1_NWR7			5
#define MR1_NWR8			6

#define MR1_VALUE	(MR1_NWR3 << 5) | (MR1_WC << 4) | (MR1_BT_SEQ << 3)  \
							| (MR1_BL8 << 0)

/* defines for MR2 */
#define MR2_RL3_WL1			1
#define MR2_RL4_WL2			2
#define MR2_RL5_WL2			3
#define MR2_RL6_WL3			4

/* defines for MR10 */
#define MR10_ZQINIT			0xFF
#define MR10_ZQRESET			0xC3
#define MR10_ZQCL			0xAB
#define MR10_ZQCS			0x56


/* TODO: FREQ update method is not working so shadow registers programming
 * is just for same of completeness. This would be safer if auto
 * trasnitions are working
 */
#define FREQ_UPDATE_EMIF
/* EMIF Needs to be configured@19.2 MHz and shadow registers
 * should be programmed for new OPP.
 */
/* Elpida 2x2Gbit */
#define SDRAM_CONFIG_INIT		0x80800EB1
#define DDR_PHY_CTRL_1_INIT		0x849FFFF5
#define READ_IDLE_CTRL			0x000501FF
#define PWR_MGMT_CTRL			0x4000000f
#define PWR_MGMT_CTRL_OPP100		0x4000000f
#define ZQ_CONFIG			0x500b3215

#define CS1_MR(mr)	((mr) | 0x80000000)

void reset_phy(unsigned int base)
{
	*(volatile int*)(base + IODFT_TLGC) |= (1 << 10);
}

/* TODO: FREQ update method is not working so shadow registers programming
 * is just for same of completeness. This would be safer if auto
 * trasnitions are working
 */
static void emif_config(unsigned int base, const struct ddr_regs *ddr_regs)
{
	unsigned int reg_value, rev;
	rev = omap_revision();

	/*
	 * set SDRAM CONFIG register
	 * EMIF_SDRAM_CONFIG[31:29] REG_SDRAM_TYPE = 4 for LPDDR2-S4
	 * EMIF_SDRAM_CONFIG[28:27] REG_IBANK_POS = 0
	 * EMIF_SDRAM_CONFIG[13:10] REG_CL = 3
	 * EMIF_SDRAM_CONFIG[6:4] REG_IBANK = 3 - 8 banks
	 * EMIF_SDRAM_CONFIG[3] REG_EBANK = 0 - CS0
 	 * EMIF_SDRAM_CONFIG[2:0] REG_PAGESIZE = 2  - 512- 9 column
	 * JDEC specs - S4-2Gb --8 banks -- R0-R13, C0-c8
	 */
	*(volatile int*)(base + EMIF_LPDDR2_NVM_CONFIG) &= 0xBFFFFFFF;
	*(volatile int*)(base + EMIF_SDRAM_CONFIG) = ddr_regs->config_init;

	/* PHY control values */
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1) = DDR_PHY_CTRL_1_INIT;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1_SHDW)= ddr_regs->phy_ctrl_1;

	/*
	 * EMIF_READ_IDLE_CTRL
	 */
	*(volatile int*)(base + EMIF_READ_IDLE_CTRL) = READ_IDLE_CTRL;
	*(volatile int*)(base + EMIF_READ_IDLE_CTRL_SHDW) = READ_IDLE_CTRL;

	/*
	 * EMIF_SDRAM_TIM_1
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_1) = ddr_regs->tim1;
	*(volatile int*)(base + EMIF_SDRAM_TIM_1_SHDW) = ddr_regs->tim1;

	/*
	 * EMIF_SDRAM_TIM_2
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_2) = ddr_regs->tim2;
	*(volatile int*)(base + EMIF_SDRAM_TIM_2_SHDW) = ddr_regs->tim2;

	/*
	 * EMIF_SDRAM_TIM_3
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_3) = ddr_regs->tim3;
	*(volatile int*)(base + EMIF_SDRAM_TIM_3_SHDW) = ddr_regs->tim3;

	*(volatile int*)(base + EMIF_ZQ_CONFIG) = ddr_regs->zq_config;
	/*
	 * EMIF_PWR_MGMT_CTRL
	 */
	//*(volatile int*)(base + EMIF_PWR_MGMT_CTRL) = PWR_MGMT_CTRL;
	//*(volatile int*)(base + EMIF_PWR_MGMT_CTRL_SHDW) = PWR_MGMT_CTRL_OPP100;
	/*
	 * poll MR0 register (DAI bit)
	 * REG_CS[31] = 0 -- Mode register command to CS0
	 * REG_REFRESH_EN[30] = 1 -- Refresh enable after MRW
	 * REG_ADDRESS[7:0] = 00 -- Refresh enable after MRW
	 */

	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) = MR0_ADDR;
	do {
		reg_value = *(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA);
	} while ((reg_value & 0x1) != 0);

	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) = CS1_MR(MR0_ADDR);
	do {
		reg_value = *(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA);
	} while ((reg_value & 0x1) != 0);


	/* set MR10 register */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR10_ADDR;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = MR10_ZQINIT;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) = CS1_MR(MR10_ADDR);
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = MR10_ZQINIT;

	/* wait for tZQINIT=1us  */
	spin_delay(10);

	/* set MR1 register */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR1_ADDR;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = ddr_regs->mr1;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) = CS1_MR(MR1_ADDR);
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = ddr_regs->mr1;


	/* set MR2 register RL=6 for OPP100 */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR2_ADDR;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = ddr_regs->mr2;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) = CS1_MR(MR2_ADDR);
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = ddr_regs->mr2;

	/* Set SDRAM CONFIG register again here with final RL-WL value */
	*(volatile int*)(base + EMIF_SDRAM_CONFIG) = ddr_regs->config_final;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1) = ddr_regs->phy_ctrl_1;

	/*
	 * EMIF_SDRAM_REF_CTRL
	 * refresh rate = DDR_CLK / reg_refresh_rate
	 * 3.9 uS = (400MHz)	/ reg_refresh_rate
	 */
	*(volatile int*)(base + EMIF_SDRAM_REF_CTRL) = ddr_regs->ref_ctrl;
	*(volatile int*)(base + EMIF_SDRAM_REF_CTRL_SHDW) = ddr_regs->ref_ctrl;

	/* set MR16 register */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR16_ADDR | REF_EN;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = 0;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) =
						 CS1_MR(MR16_ADDR | REF_EN);
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = 0;
	/* LPDDR2 init complete */

}
/*****************************************
 * Routine: ddr_init
 * Description: Configure DDR
 * EMIF1 -- CS0 -- DDR1 (256 MB)
 * EMIF2 -- CS0 -- DDR2 (256 MB)
 *****************************************/
void do_ddr_init(const struct ddr_regs *emif1_ddr_regs,
		 const struct ddr_regs *emif2_ddr_regs)
{
	unsigned int rev;
	rev = omap_revision();

	if (rev == OMAP4430_ES1_0)
	{
		/* Configurte the Control Module DDRIO device */
		__raw_writel(0x1c1c1c1c, 0x4A100638);
		__raw_writel(0x1c1c1c1c, 0x4A10063c);
		__raw_writel(0x1c1c1c1c, 0x4A100640);
		__raw_writel(0x1c1c1c1c, 0x4A100648);
		__raw_writel(0x1c1c1c1c, 0x4A10064c);
		__raw_writel(0x1c1c1c1c, 0x4A100650);
		/* LPDDR2IO set to NMOS PTV */
		__raw_writel(0x00ffc000, 0x4A100704);
	} else if (rev == OMAP4430_ES2_0) {
		__raw_writel(0x9e9e9e9e, 0x4A100638);
		__raw_writel(0x9e9e9e9e, 0x4A10063c);
		__raw_writel(0x9e9e9e9e, 0x4A100640);
		__raw_writel(0x9e9e9e9e, 0x4A100648);
		__raw_writel(0x9e9e9e9e, 0x4A10064c);
		__raw_writel(0x9e9e9e9e, 0x4A100650);
		/* LPDDR2IO set to NMOS PTV */
		__raw_writel(0x00ffc000, 0x4A100704);
	}
	/* EMIF2 only at 0x90000000 */
	//*(volatile int*)(DMM_BASE + DMM_LISA_MAP_1) = 0x90400200;

	*(volatile int*)(DMM_BASE + DMM_LISA_MAP_2) = 0x00000000;
	*(volatile int*)(DMM_BASE + DMM_LISA_MAP_3) = 0xFF020100;

	/* DDR needs to be initialised @ 19.2 MHz
	 * So put core DPLL in bypass mode
	 * Configure the Core DPLL but don't lock it
	 */
	configure_core_dpll_no_lock();

	/* No IDLE: BUG in SDC */
	//sr32(CM_MEMIF_CLKSTCTRL, 0, 32, 0x2);
	//while(((*(volatile int*)CM_MEMIF_CLKSTCTRL) & 0x700) != 0x700);
	*(volatile int*)(EMIF1_BASE + EMIF_PWR_MGMT_CTRL) = 0x0;
	*(volatile int*)(EMIF2_BASE + EMIF_PWR_MGMT_CTRL) = 0x0;

	/* Configure EMIF1 */
	emif_config(EMIF1_BASE, emif1_ddr_regs);

	/* Configure EMIF2 */
	emif_config(EMIF2_BASE, emif2_ddr_regs);
	/* Lock Core using shadow CM_SHADOW_FREQ_CONFIG1 */
	lock_core_dpll_shadow();
	/* TODO: SDC needs few hacks to get DDR freq update working */

	/* Set DLL_OVERRIDE = 0 */
	*(volatile int*)CM_DLL_CTRL = 0x0;

	spin_delay(200);

	/* Check for DDR PHY ready for EMIF1 & EMIF2 */
	while((((*(volatile int*)(EMIF1_BASE + EMIF_STATUS))&(0x04)) != 0x04) \
	|| (((*(volatile int*)(EMIF2_BASE + EMIF_STATUS))&(0x04)) != 0x04));

	/* Reprogram the DDR PYHY Control register */
	/* PHY control values */

	sr32(CM_MEMIF_EMIF_1_CLKCTRL, 0, 32, 0x1);
        sr32(CM_MEMIF_EMIF_2_CLKCTRL, 0, 32, 0x1);

	/* Put the Core Subsystem PD to ON State */

	/* No IDLE: BUG in SDC */
	//sr32(CM_MEMIF_CLKSTCTRL, 0, 32, 0x2);
	//while(((*(volatile int*)CM_MEMIF_CLKSTCTRL) & 0x700) != 0x700);
	*(volatile int*)(EMIF1_BASE + EMIF_PWR_MGMT_CTRL) = 0x80000000;
	*(volatile int*)(EMIF2_BASE + EMIF_PWR_MGMT_CTRL) = 0x80000000;

	/* SYSTEM BUG:
	 * In n a specific situation, the OCP interface between the DMM and
	 * EMIF may hang.
	 * 1. A TILER port is used to perform 2D burst writes of
	 * 	 width 1 and height 8
	 * 2. ELLAn port is used to perform reads
	 * 3. All accesses are routed to the same EMIF controller
	 *
	 * Work around to avoid this issue REG_SYS_THRESH_MAX value should
	 * be kept higher than default 0x7. As per recommondation 0x0A will
	 * be used for better performance with REG_LL_THRESH_MAX = 0x00
	 */
	if (rev == OMAP4430_ES1_0) {
		*(volatile int*)(EMIF1_BASE + EMIF_L3_CONFIG) = 0x0A0000FF;
		*(volatile int*)(EMIF2_BASE + EMIF_L3_CONFIG) = 0x0A0000FF;
	}

	/*
	 * DMM : DMM_LISA_MAP_0(Section_0)
	 * [31:24] SYS_ADDR 		0x80
	 * [22:20] SYS_SIZE		0x7 - 2Gb
	 * [19:18] SDRC_INTLDMM		0x1 - 128 byte
	 * [17:16] SDRC_ADDRSPC 	0x0
	 * [9:8] SDRC_MAP 		0x3
	 * [7:0] SDRC_ADDR		0X0
	 */

	reset_phy(EMIF1_BASE);
	reset_phy(EMIF2_BASE);

	*((volatile int *)0x80000000) = 0;
	*((volatile int *)0x80000080) = 0;
	//*((volatile int *)0x90000000) = 0;
}

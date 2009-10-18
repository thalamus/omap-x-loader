/*
 * (C) Copyright 2004-2009
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
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
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <asm/arch/clocks.h>
#include <asm/arch/mem.h>
#include <i2c.h>
#include <asm/mach-types.h>
#if (CONFIG_COMMANDS & CFG_CMD_NAND) && defined(CFG_NAND_LEGACY)
#include <linux/mtd/nand_legacy.h>
#endif

#define CONFIG_OMAP4_SDC		1

/* EMIF and DMM registers */
#define EMIF1_BASE			0x4c000000
#define EMIF2_BASE			0x4d000000
#define DMM_BASE			0x4e000000
/* EMIF */
#define EMIF_MOD_ID_REV			0x0000
#define EMIF_STATUS			0x0004
#define EMIF_SDRAM_CONFIG		0x0008
#define EMIF_LPDDR2_NVM_CONFIG		0x000C
#define EMIF_SDRAM_REF_CTRL		0x0010
#define EMIF_SDRAM_REF_CTRL_SHDW	0x0014
#define EMIF_SDRAM_TIM_1		0x0018
#define EMIF_SDRAM_TIM_1_SHDW		0x001C
#define EMIF_SDRAM_TIM_2		0x0020
#define EMIF_SDRAM_TIM_2_SHDW		0x0024
#define EMIF_SDRAM_TIM_3		0x0028
#define EMIF_SDRAM_TIM_3_SHDW		0x002C
#define EMIF_LPDDR2_NVM_TIM		0x0030
#define EMIF_LPDDR2_NVM_TIM_SHDW	0x0034
#define EMIF_PWR_MGMT_CTRL		0x0038
#define EMIF_PWR_MGMT_CTRL_SHDW		0x003C
#define EMIF_LPDDR2_MODE_REG_DATA	0x0040
#define EMIF_LPDDR2_MODE_REG_CFG	0x0050
#define EMIF_L3_CONFIG			0x0054
#define EMIF_L3_CFG_VAL_1		0x0058
#define EMIF_L3_CFG_VAL_2		0x005C
#define EMIF_PERF_CNT_1			0x0080
#define EMIF_PERF_CNT_2			0x0084
#define EMIF_PERF_CNT_CFG		0x0088
#define EMIF_PERF_CNT_SEL		0x008C
#define EMIF_PERF_CNT_TIM		0x0090
#define EMIF_READ_IDLE_CTRL		0x0098
#define EMIF_READ_IDLE_CTRL_SHDW	0x009c
#define EMIF_ZQ_CONFIG			0x00C8
#define EMIF_DDR_PHY_CTRL_1		0x00E4
#define EMIF_DDR_PHY_CTRL_1_SHDW	0x00E8
#define EMIF_DDR_PHY_CTRL_2		0x00EC

#define DMM_LISA_MAP_0 			0x0040
#define DMM_LISA_MAP_1 			0x0044
#define DMM_LISA_MAP_2 			0x0048
#define DMM_LISA_MAP_3 			0x004C

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
//#define FREQ_UPDATE_EMIF

/* EMIF Needs to be configured@19.2 MHz and shadow registers
 * should be programmed for new OPP.
 */
/* Elpida 2x2Gbit */
#ifdef CONFIG_OMAP4_SDC
	/*
	 * EMIF_SDRAM_REF_CTRL
	 * refresh rate = DDR_CLK / reg_refresh_rate
	 * 1/3.9 uS = (333MHz)	/ reg_refresh_rate
	 */
#define SDRAM_REF_CTRL			0x0000004A
#define SDRAM_REF_CTRL_OPP100		0x0000050E
/*
 *	28:25 REG_T_RP 	Minimum number of m_clk cycles from
 *			Precharge to Activate or Refresh, minus one.
 *	24:21 REG_T_RCD	Minimum number of m_clk cycles from
 *			Activate to Read or Write, minus one.
 *	20:17 REG_T_WR	Minimum number of m_clk cycles from last
 *			Write transfer to Pre-charge, minus one.
 *	16:12 REG_T_RAS	Minimum number of m_clk cycles from Activate
 *			to Pre-charge, minus one. reg_t_ras value need
 *			to be bigger than or equal to reg_t_rcd value.
 *	11:6 REG_T_RC 	Minimum number of m_clk cycles from
 *			Activate to Activate, minus one.
 *	5:3 REG_T_RRD 	Minimum number of m_clk cycles from
 *			Activate to Activate for a different bank, minus one.
 *			For an 8-bank, this field must be equal to
 *			((tFAW/(4*tCK))-1).
 *	2:0 REG_T_WTR 	Minimum number of m_clk cycles from last Write
 */
#define SDRAM_TIM_1			0x04442049
#define SDRAM_TIM_1_OPP100		0x0CA8D51A

/*
 *	30:28 REG_T_XP		Minimum number of m_clk cycles from
 *				Powerdown exit to any command other than a
 *				Read command, minus one.
 *	24:16 REG_T_XSNR	Minimum number of m_clk cycles from Self-Refresh
 *				exit to any command other than a Read command,
 *				minusone. REG_T_XSNR and REG_T_XSRD must be
 *				programmed with the same value.
 *	15:6 REG_T_XSRD		Minimum number of m_clk cycles from Self-Refresh
 *				exit to a Read command,
 *				minus one. REG_T_XSNR and REG_T_XSRD must be
 *				programmed with the same value.
 *	5:3 REG_T_RTP		Minimum number of m_clk cycles for the last
 *				read command to a Pre-charge command, minus one.
 */
#define SDRAM_TIM_2			0x1002008A
#define SDRAM_TIM_2_OPP100		0x202E0B92

/*
 *	23:21 REG_T_CKESR	Minimum number of m_clk cycles for which LPDDR2
 *				must remain in Self Refresh, minus one.
 *	20:15 REG_ZQ_ZQCS 	Number of m_clk clock cycles for a ZQCS command
 *				minus one.
 *	14:13 REG_T_TDQSCKMAX 	Number of m_clk that satisfies tDQSCKmax for
 *				LPDDR2,minus one.
 *	12:4  REG_T_RFC 	Minimum number of m_clk cycles from Refresh or
 *				 Load
 *				Mode to Refresh or Activate, minus one.
 *	3:0 REG_T_RAS_MAX 	Maximum number of reg_refresh_rate intervals
 *				from Activate to Precharge command. This field
 *				must be equal to ((tRASmax / tREFI)-1)
 *				rounded down to the next lower integer.
 *				Value for REG_T_RAS_MAX can be calculated as
 *				follows:
 *				If tRASmax = 120 us and tREFI = 15.7 us, then
 *				REG_T_RAS_MAX = ((120/15.7)-1) = 6.64.
 *				Round down to the next lower integer.
 *				Therefore, the programmed value must be 6
 */
#define SDRAM_TIM_3			0x0040802F
#define SDRAM_TIM_3_OPP100		0x008EA2BF
#define SDRAM_CONFIG_INIT		0x80800EB1
#define SDRAM_CONFIG_FINAL		0x80801AB1
#define DDR_PHY_CTRL_1_INIT		0x849FFFF4
#define DDR_PHY_CTRL_1_OPP100_INIT	0x849FF404
#define DDR_PHY_CTRL_1_FINAL		0x849FFFF8
#define DDR_PHY_CTRL_1_OPP100_FINAL	0x849FF408
#define DDR_PHY_CTRL_2			0x00000000
#define READ_IDLE_CTRL			0x000501FF
#define READ_IDLE_CTRL_OPP100		0x000501FF
#define PWR_MGMT_CTRL			0x80000000
#define PWR_MGMT_CTRL_OPP100		0x00000000

#else
/* TODO: ES1.0 OPP100 valuse are still not popullated
 * 600 MHz/200 MHz
 */
#define SDRAM_REF_CTRL			0x0000004A
#define SDRAM_REF_CTRL_OPP100		0x0000050E
#define SDRAM_TIM_1			0x04442049
#define SDRAM_TIM_1_OPP100		0x0CA8D51A
#define SDRAM_TIM_2			0x1002008A
#define SDRAM_TIM_2_OPP100		0x202E0B92
#define SDRAM_TIM_3			0x0040802F
#define SDRAM_TIM_3_OPP100		0x008EA2BF
#define SDRAM_CONFIG_INIT		0x80800EB1
#define SDRAM_CONFIG_FINAL		0x80801AB1
#define DDR_PHY_CTRL_1_INIT		0x849FFFF4
#define DDR_PHY_CTRL_1_OPP100_INIT	0x849FF404
#define DDR_PHY_CTRL_1_FINAL		0x849FFFF8
#define DDR_PHY_CTRL_1_OPP100_FINAL	0x849FF408
#define DDR_PHY_CTRL_2			0x00000000
#define READ_IDLE_CTRL			0x000501FF
#define READ_IDLE_CTRL_OPP100		0x000501FF
#define PWR_MGMT_CTRL			0x80000000
#define PWR_MGMT_CTRL_OPP100		0x00000000

#endif


/*******************************************************
 * Routine: delay
 * Description: spinning delay to use before udelay works
 ******************************************************/
static inline void delay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b" : "=r" (loops) : "0"(loops));
}

/* TODO: FREQ update method is not working so shadow registers programming
 * is just for same of completeness. This would be safer if auto
 * trasnitions are working
 */
static int emif_config(unsigned int base)
{
	unsigned int reg_value;

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
	*(volatile int*)(base + EMIF_SDRAM_CONFIG) = SDRAM_CONFIG_INIT;

	/* PHY control values */
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1) = DDR_PHY_CTRL_1_INIT;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1_SHDW)= 		\
						DDR_PHY_CTRL_1_OPP100_INIT;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_2) = DDR_PHY_CTRL_2;

	/*
	 * EMIF_READ_IDLE_CTRL
	 */
	*(volatile int*)(base + EMIF_READ_IDLE_CTRL) = READ_IDLE_CTRL;
	*(volatile int*)(base + EMIF_READ_IDLE_CTRL_SHDW) = READ_IDLE_CTRL_OPP100;

	/*
	 * EMIF_SDRAM_TIM_1
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_1) = SDRAM_TIM_1;
	*(volatile int*)(base + EMIF_SDRAM_TIM_1_SHDW) = SDRAM_TIM_1_OPP100;

	/*
	 * EMIF_SDRAM_TIM_2
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_2) = SDRAM_TIM_2;
	*(volatile int*)(base + EMIF_SDRAM_TIM_2_SHDW) = SDRAM_TIM_2_OPP100;

	/*
	 * EMIF_SDRAM_TIM_3
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_3) = SDRAM_TIM_3;
	*(volatile int*)(base + EMIF_SDRAM_TIM_3_SHDW) = SDRAM_TIM_3_OPP100;

	/*
	 * EMIF_PWR_MGMT_CTRL
	 */
	*(volatile int*)(base + EMIF_PWR_MGMT_CTRL) = PWR_MGMT_CTRL;
	*(volatile int*)(base + EMIF_PWR_MGMT_CTRL_SHDW) = PWR_MGMT_CTRL_OPP100;
	/*
	 * poll MR0 register (DAI bit)
	 * REG_CS[31] = 0 -- Mode register command to CS0
	 * REG_REFRESH_EN[30] = 1 -- Refresh enable after MRW
	 * REG_ADDRESS[7:0] = 00 -- Refresh enable after MRW
	 */

	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG) = MR0_ADDR;
	do
	{
		reg_value = *(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA);
	} while((reg_value & 0x1) != 0);

	/* set MR10 register */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR10_ADDR;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = MR10_ZQINIT;
	/* wait for tZQINIT=1us  */
	delay(10);

	/* set MR1 register */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR1_ADDR;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = MR1_VALUE;

	/* set MR2 register RL=6 for OPP100 */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR2_ADDR;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = MR2_RL6_WL3;

	/* Set SDRAM CONFIG register again here with final RL-WL value */
	*(volatile int*)(base + EMIF_SDRAM_CONFIG) = SDRAM_CONFIG_FINAL;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1) = DDR_PHY_CTRL_1_FINAL;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1_SHDW)= 		\
						DDR_PHY_CTRL_1_OPP100_FINAL;

	/*
	 * EMIF_SDRAM_REF_CTRL
	 * refresh rate = DDR_CLK / reg_refresh_rate
	 * 3.9 uS = (400MHz)	/ reg_refresh_rate
	 */
	*(volatile int*)(base + EMIF_SDRAM_REF_CTRL) = SDRAM_REF_CTRL;
	*(volatile int*)(base + EMIF_SDRAM_REF_CTRL_SHDW) = 		\
							SDRAM_REF_CTRL_OPP100;

	/* set MR16 register */
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_CFG)= MR16_ADDR | REF_EN;
	*(volatile int*)(base + EMIF_LPDDR2_MODE_REG_DATA) = 0;
	/* LPDDR2 init complete */

}
/* FREQ update method is not working so use Normal Reconfigure method
 * Shadow registers are programmed for completeness already in emif_config
 * fucntion @ 100 OPP
 * This fucntion popullated the 100% OPP values
 */
static int emif_reconfig(unsigned int base)
{
	unsigned int reg_value;

	*(volatile int*)(base + EMIF_SDRAM_CONFIG) = SDRAM_CONFIG_FINAL;

	/* PHY control values */
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_1) 			\
						= DDR_PHY_CTRL_1_OPP100_FINAL;
	*(volatile int*)(base + EMIF_DDR_PHY_CTRL_2) = DDR_PHY_CTRL_2;

	/*
	 * EMIF_READ_IDLE_CTRL
	 */
	*(volatile int*)(base + EMIF_READ_IDLE_CTRL) = READ_IDLE_CTRL_OPP100;

	/*
	 * EMIF_SDRAM_TIM_1
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_1) = SDRAM_TIM_1_OPP100;

	/*
	 * EMIF_SDRAM_TIM_2
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_2) = SDRAM_TIM_2_OPP100;

	/*
	 * EMIF_SDRAM_TIM_3
	 */
	*(volatile int*)(base + EMIF_SDRAM_TIM_3) = SDRAM_TIM_3_OPP100;

	/*
	 * EMIF_PWR_MGMT_CTRL
	 */
	*(volatile int*)(base + EMIF_PWR_MGMT_CTRL) = PWR_MGMT_CTRL_OPP100;

	/*
	 * EMIF_SDRAM_REF_CTRL
	 * refresh rate = DDR_CLK / reg_refresh_rate
	 * 3.9 uS = (400MHz)	/ reg_refresh_rate
	 */
	*(volatile int*)(base + EMIF_SDRAM_REF_CTRL) = SDRAM_REF_CTRL_OPP100;

	/* LPDDR2 init complete at 100 OPP*/

}
/*****************************************
 * Routine: ddr_init
 * Description: Configure DDR
 * EMIF1 -- CS0 -- DDR1 (256 MB)
 * EMIF2 -- CS0 -- DDR2 (256 MB)
 *****************************************/
static void ddr_init(void)
{
	unsigned int base_addr;

	/* DDR needs to be initialised @ 19.2 MHz
	 * So put core DPLL in bypass mode
	 * Configure the Core DPLL but don't lock it
	 */
	configure_core_dpll_no_lock();

	/* No IDLE: BUG in SDC */
	sr32(CM_MEMIF_CLKSTCTRL, 0, 32, 0x2);
	while(((*(volatile int*)CM_MEMIF_CLKSTCTRL) & 0x700) != 0x700);
	/* Configure EMIF14D */
	base_addr = EMIF1_BASE;
	emif_config(base_addr);

	/* Configure EMIF24D */
	base_addr = EMIF2_BASE;
	emif_config(base_addr);
#ifdef FREQ_UPDATE_EMIF
	/* Lock Core using shadow CM_SHADOW_FREQ_CONFIG1 */
	lock_core_dpll_shadow();
#else
	/* Lock Core dpll
	 * FREQ update method is not working
	 * so use generic approach
	 */
	lock_core_dpll();

	/* Reconfigure EMIF14D */
	base_addr = EMIF1_BASE;
	emif_reconfig(base_addr);

	/* Configure EMIF24D */
	base_addr = EMIF2_BASE;
	emif_reconfig(base_addr);

	/* Set DLL_OVERRIDE = 0 */
	*(volatile int*)CM_DLL_CTRL = 0x0;

	/* Check for DDR PHY ready for EMIF1 & EMIF2 */
	while((((*(volatile int*)EMIF1_BASE + EMIF_STATUS)&(0x04)) != 0x04) \
	|| (((*(volatile int*)EMIF2_BASE + EMIF_STATUS)&(0x04)) != 0x04));

#endif	/* sr32(CM_MEMIF_CLKSTCTRL, 0, 32, 0x3); */ /* SDC BUG */
	sr32(CM_MEMIF_EMIF_1_CLKCTRL, 0, 32, 0x1);
        sr32(CM_MEMIF_EMIF_2_CLKCTRL, 0, 32, 0x1);

	/* Put the Core Subsystem PD to ON State */
	sr32(CM_MEMIF_EMIF_2_CLKCTRL, 0, 32, 0x30E03);
	/*
	 * DMM : DMM_LISA_MAP_0(Section_0)
	 * [31:24] SYS_ADDR 		0x80
	 * [22:20] SYS_SIZE		0x7 - 2Gb
	 * [19:18] SDRC_INTLDMM		0x1 - 128 byte
	 * [17:16] SDRC_ADDRSPC 	0x0
	 * [9:8] SDRC_MAP 		0x3
	 * [7:0] SDRC_ADDR		0X0
	 */
	// *(volatile int*)(DMM_BASE + DMM_LISA_MAP_0) = 0x80700100;
	*(volatile int*)(DMM_BASE + DMM_LISA_MAP_0) = 0x80540300;
	/* TODO: Settings can be locked but kept open for TILER */
	*(volatile int*)(DMM_BASE + DMM_LISA_MAP_1) = 0x00000000;
	*(volatile int*)(DMM_BASE + DMM_LISA_MAP_2) = 0x00000000;
	/* Invalid address TRAP */
	*(volatile int*)(DMM_BASE + DMM_LISA_MAP_3) = 0xFF020100;

}
/*****************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************/
int board_init(void)
{
	return 0;
}

/*****************************************
 * Routine: secure_unlock
 * Description: Setup security registers for access
 * (GP Device only)
 *****************************************/
void secure_unlock_mem(void)
{
	/* Permission values for registers -Full fledged permissions to all */
	#define UNLOCK_1 0xFFFFFFFF
	#define UNLOCK_2 0x00000000
	#define UNLOCK_3 0x0000FFFF

	/* Protection Module Register Target APE (PM_RT)*/
	__raw_writel(UNLOCK_1, RT_REQ_INFO_PERMISSION_1);
	__raw_writel(UNLOCK_1, RT_READ_PERMISSION_0);
	__raw_writel(UNLOCK_1, RT_WRITE_PERMISSION_0);
	__raw_writel(UNLOCK_2, RT_ADDR_MATCH_1);

	__raw_writel(UNLOCK_3, GPMC_REQ_INFO_PERMISSION_0);
	__raw_writel(UNLOCK_3, GPMC_READ_PERMISSION_0);
	__raw_writel(UNLOCK_3, GPMC_WRITE_PERMISSION_0);

	__raw_writel(UNLOCK_3, OCM_REQ_INFO_PERMISSION_0);
	__raw_writel(UNLOCK_3, OCM_READ_PERMISSION_0);
	__raw_writel(UNLOCK_3, OCM_WRITE_PERMISSION_0);
	__raw_writel(UNLOCK_2, OCM_ADDR_MATCH_2);

	/* IVA Changes */
	__raw_writel(UNLOCK_3, IVA2_REQ_INFO_PERMISSION_0);
	__raw_writel(UNLOCK_3, IVA2_READ_PERMISSION_0);
	__raw_writel(UNLOCK_3, IVA2_WRITE_PERMISSION_0);

	__raw_writel(UNLOCK_1, SMS_RG_ATT0); /* SDRC region 0 public */
}

/**********************************************************
 * Routine: try_unlock_sram()
 * Description: If chip is GP/EMU(special) type, unlock the SRAM for
 *  general use.
 ***********************************************************/
void try_unlock_memory(void)
{
	int mode;

	/* if GP device unlock device SRAM for general use */
	/* secure code breaks for Secure/Emulation device - HS/E/T*/
	return;
}

/**********************************************************
 * Routine: s_init
 * Description: Does early system init of muxing and clocks.
 * - Called path is with SRAM stack.
 **********************************************************/

void s_init(void)
{
	set_muxconf_regs();
	delay(100);

	/* Writing to AuxCR in U-boot using SMI for GP/EMU DEV */
	/* Currently SMI in Kernel on ES2 devices seems to have an isse
	 * Once that is resolved, we can postpone this config to kernel
	 */
	//setup_auxcr(get_device_type(), external_boot);

	ddr_init();

	prcm_init();

}

/*******************************************************
 * Routine: misc_init_r
 * Description: Init ethernet (done here so udelay works)
 ********************************************************/
int misc_init_r(void)
{
	return 0;
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

/*******************************************************************
 * Routine:ether_init
 * Description: take the Ethernet controller out of reset and wait
 *  		   for the EEPROM load to complete.
 ******************************************************************/

/**********************************************
 * Routine: dram_init
 * Description: sets uboots idea of sdram size
 **********************************************/
int dram_init(void)
{
	return 0;
}


#define		CP(x)	(CONTROL_PADCONF_##x)
/*
 * IEN  - Input Enable
 * IDIS - Input Disable
 * PTD  - Pull type Down
 * PTU  - Pull type Up
 * DIS  - Pull type selection is inactive
 * EN   - Pull type selection is active
 * M0   - Mode 0
 * The commented string gives the final mux configuration for that pin
 */


/**********************************************************
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers
 *              specific to the hardware. Many pins need
 *              to be moved from protect to primary mode.
 *********************************************************/
void set_muxconf_regs(void)
{
       /* REVISIT */
	return;
}

/******************************************************************************
 * Routine: update_mux()
 * Description:Update balls which are different between boards.  All should be
 *             updated to match functionality.  However, I'm only updating ones
 *             which I'll be using for now.  When power comes into play they
 *             all need updating.
 *****************************************************************************/
void update_mux(u32 btype, u32 mtype)
{
	/* REVISIT  */
	return;

}

/* optionally do something like blinking LED */
void board_hang (void)
{ while (0) {};}

int nand_init(void)
{
	return 1;
}

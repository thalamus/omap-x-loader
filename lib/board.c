/*
 * Copyright (C) 2005 Texas Instruments.
 *
 * (C) Copyright 2004
 * Jian Zhang, Texas Instruments, jzhang@ti.com.
 *
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <part.h>
#include <fat.h>
#include <mmc.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/clocks.h>
#include <asm/arch/sys_proto.h>


#ifdef CFG_PRINTF
int print_info(void)
{
	printf ("\n\nTexas Instruments X-Loader 1.41 ("
		__DATE__ " - " __TIME__ ")\n");
	return 0;
}
#endif
typedef int (init_fnc_t) (void);

init_fnc_t *init_sequence[] = {
	cpu_init,		/* basic cpu dependent setup */
	board_init,		/* basic board dependent setup */
#ifdef CFG_PRINTF
 	serial_init,		/* serial communications setup */
	print_info,
#endif
   	//nand_init,		/* board specific nand init */
	NULL,
};

#ifdef CFG_CMD_FAT
extern char * strcpy(char * dest,const char *src);
#else
char * strcpy(char * dest,const char *src)
{
	 char *tmp = dest;

	 while ((*dest++ = *src++) != '\0')
	         /* nothing */;
	 return tmp;
}
#endif

#ifdef CFG_CMD_MMC
extern block_dev_desc_t *mmc_get_dev(int dev);
int mmc_read_bootloader(int dev)
{
	unsigned char ret = 0;
	unsigned long offset = CFG_LOADADDR;

	ret = mmc_init(dev);
	if (ret != 0){
		printf("\n MMC init failed \n");
		return -1;
	}

#ifdef CFG_CMD_FAT
	long size;
	block_dev_desc_t *dev_desc = NULL;

	if (fat_boot()) {
		dev_desc = mmc_get_dev(dev);
		fat_register_device(dev_desc, 1);
		size = file_fat_read("u-boot.bin", (unsigned char *)offset, 0);
		if (size == -1)
			return -1;
	} else {
		/* FIXME: OMAP4 specific */
		 mmc_read(dev, 0x200, (unsigned char *)CFG_LOADADDR,
							0x00060000);
	}
#endif
	return 0;
}
#endif

/*
 * OMAP On-die temperature sensor check.
 * If the current temperature value is
 * greater than T_SHUT_HOT stop boot
 */

void omap_temp_sensor_check(void)
{
	u32 temp;

	/* Set the counter to 1 ms */
	sr32(CORE_BANDGAP_COUNTER, BGAP_COUNTER_START_BIT,
			BGAP_COUNTER_NUM_BITS, BGAP_COUNTER_VALUE);

	/* Enable continuous mode. */
	sr32(CORE_BANDGAP_CTRL, BGAP_SINGLE_MODE_START_BIT,
			BGAP_SINGLE_MODE_NUM_BITS, BGAP_CONTINUOUS_MODE);

	/* Wait till the first conversion is done wait for at least 1ms */
	spin_delay(20000);

	/* Read the temperature adc_value */
	temp = readl(CORE_TEMP_SENSOR);
	temp = temp & BGAP_TEMP_SENSOR_DTEMP_MASK;

	/* If the samples are untrimmed divide by 1.2 */
	if (readl(STD_FUSE_OPP_BGAP) == 0)
		temp = temp * 5 / 6;

	/*
	 * Compare with TSHUT high temperature. If high ask the
	 * user to shut down and restart after sometime else
	 * Disable continuous mode.
	 */
	if (temp < TSHUT_HIGH_ADC_CODE) {
		/* Disable contiuous mode */
		sr32(CORE_BANDGAP_CTRL, BGAP_SINGLE_MODE_START_BIT,
			BGAP_SINGLE_MODE_NUM_BITS, ~BGAP_CONTINUOUS_MODE);
	} else {
		printf("OMAP chip temperature is too high!!!\n");
		printf("Please power off and try booting after sometime\n");

		/* Bypass MPU, CORE, IVA, PER, ABE, USB DPLLs */
		sr32(CM_CLKMODE_DPLL_MPU, 0, 3, PLL_FAST_RELOCK_BYPASS);
		wait_on_value(BIT0, 0, CM_IDLEST_DPLL_MPU, LDELAY);

		sr32(CM_CLKMODE_DPLL_CORE, 0, 3, PLL_FAST_RELOCK_BYPASS);
		wait_on_value(BIT0, 0, CM_IDLEST_DPLL_CORE, LDELAY);

		sr32(CM_CLKMODE_DPLL_IVA, 0, 3, PLL_FAST_RELOCK_BYPASS);
		wait_on_value(BIT0, 0, CM_IDLEST_DPLL_IVA, LDELAY);

		sr32(CM_CLKMODE_DPLL_PER, 0, 3, PLL_FAST_RELOCK_BYPASS);
		wait_on_value(BIT0, 0, CM_IDLEST_DPLL_PER, LDELAY);

		sr32(CM_CLKMODE_DPLL_ABE, 0, 3, PLL_FAST_RELOCK_BYPASS);
		wait_on_value(BIT0, 0, CM_IDLEST_DPLL_ABE, LDELAY);

		sr32(CM_CLKMODE_DPLL_USB, 0, 3, PLL_FAST_RELOCK_BYPASS);
		wait_on_value(BIT0, 0, CM_IDLEST_DPLL_USB, LDELAY);

		while (1);
	}
}


extern int do_load_serial_bin(ulong offset, int baudrate);

#define __raw_readl(a)	(*(volatile unsigned int *)(a))

void start_armboot (void)
{
  	init_fnc_t **init_fnc_ptr;
	uchar *buf;
	char boot_dev_name[8];
	u32 si_type, omap4_rev;
 
   	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}

	omap4_rev = omap_revision();
	if (omap4_rev >= OMAP4460_ES1_0) {
		omap_temp_sensor_check();
		si_type = omap4_silicon_type();
		if (si_type == PROD_ID_1_SILICON_TYPE_HIGH_PERF)
			printf("OMAP4460: 1.5 GHz capable SOM\n");
		else if (si_type == PROD_ID_1_SILICON_TYPE_STD_PERF)
			printf("OMAP4460: 1.2 GHz capable SOM\n");
	}
#ifdef START_LOADB_DOWNLOAD
	strcpy(boot_dev_name, "UART");
	do_load_serial_bin (CFG_LOADADDR, 115200);
#else
	buf = (uchar *) CFG_LOADADDR;

	switch (get_boot_device()) {
	case 0x03:
		strcpy(boot_dev_name, "ONENAND");
#if defined(CFG_ONENAND)
		for (i = ONENAND_START_BLOCK; i < ONENAND_END_BLOCK; i++) {
			if (!onenand_read_block(buf, i))
				buf += ONENAND_BLOCK_SIZE;
			else
				goto error;
		}
#endif
		break;
	case 0x02:
	default:
		strcpy(boot_dev_name, "NAND");
#if defined(CFG_NAND)
		for (i = NAND_UBOOT_START; i < NAND_UBOOT_END;
				i+= NAND_BLOCK_SIZE) {
			if (!nand_read_block(buf, i))
				buf += NAND_BLOCK_SIZE; /* advance buf ptr */
		}
#endif
		break;
	case 0x05:
		strcpy(boot_dev_name, "MMC/SD1");
#if defined(CONFIG_MMC)
		if (mmc_read_bootloader(0) != 0)
			goto error;
#endif
		break;
	case 0x06:
		strcpy(boot_dev_name, "EMMC");
#if defined(CONFIG_MMC)
		if (mmc_read_bootloader(1) != 0)
			goto error;
#endif
		break;
	};
#endif
	/* go run U-Boot and never return */
	printf("Starting OS Bootloader from %s ...\n", boot_dev_name);
 	((init_fnc_t *)CFG_LOADADDR)();

	/* should never come here */
#if defined(CFG_ONENAND) || defined(CONFIG_MMC)
error:
#endif
	printf("Could not read bootloader!\n");
	hang();
}

void hang (void)
{
	/* call board specific hang function */
	board_hang();
	
	/* if board_hang() returns, hange here */
	printf("X-Loader hangs\n");
	for (;;);
}

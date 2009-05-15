/*
 * Copyright (c) 2009 Wind River Systems, Inc.
 * Tom Rix <Tom.Rix@windriver.com>
 *
 * Derived from board specific omap code by
 * Richard Woodruff <r-woodruff2@ti.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/sys_info.h>
#include <asm/arch/rev.h>

#define __raw_readl(a)    (*(volatile unsigned int *)(a))

/*
 * get_cpu_rev(void) - extract version info
 */
u32 get_cpu_rev(void)
{
	u32 cpuid = 0;
	ctrl_id_t *id_base;
	/*
	 * On ES1.0 the IDCODE register is not exposed on L4
	 * so using CPU ID to differentiate between ES1.0 and > ES1.0.
	 */
	__asm__ __volatile__("mrc p15, 0, %0, c0, c0, 0":"=r" (cpuid));
	if ((cpuid  & 0xf) == 0x0)
		return CPU_3XX_ES10;
	else {
		/* Decode the IDs on > ES1.0 */
		id_base = (ctrl_id_t *) OMAP34XX_ID_L4_IO_BASE;

		cpuid = (__raw_readl(&id_base->idcode) >> CPU_3XX_ID_SHIFT) & 0xf;

		/* Some early ES2.0 seem to report ID 0, fix this */
		if (cpuid == 0)
			cpuid = CPU_3XX_ES20;

		return cpuid;
	}
}

/*
 * cpu_is_3410(void) - returns true for 3410
 */
u32 cpu_is_3410(void)
{
	int status;
	if(get_cpu_rev() < CPU_3XX_ES20) {
		return 0;
	} else {
		/* read scalability status and return 1 for 3410*/
		status = __raw_readl(CONTROL_SCALABLE_OMAP_STATUS);
		/* Check whether MPU frequency is set to 266 MHz which
		 * is nominal for 3410. If yes return true else false
		 */
		if (((status >> 8) & 0x3) == 0x2)
			return 1;
		else
			return 0;
	}
}


/*
 * ROM Code HAL apis
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
#ifndef __OMAP4_ROM_HAL_API_H
#define __OMAP4_ROM_HAL_API_H

/* Defines for ROM Services */
#define ROM_SERVICE_PL310_AUXCR		0x109
#define ROM_SERVICE_PL310_POR		0x113

/*
 * Define only official TI *REQUIRED* PPA services here.
 * 	PPA_SERVICE_xyz
 *
 * IMPORTANT: PPA service ID may change based on PPA used,
 */
#define PPA_SERVICE_PL310_POR		0x23

#ifndef    __ASSEMBLY__
/* Function to call a Public ROM code service */
extern unsigned int omap_smc_rom(unsigned int rom_svc,
		unsigned int arg);
#endif		/* __ASSEMBLY__ */

#endif		/* __OMAP4_ROM_HAL_API_H_ */

/* ____________________________________________________________________________
*
*            TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION
*
*  Property of Texas Instruments
*  For Unrestricted Internal Use Only
*  Unauthorized reproduction and/or distribution is strictly prohibited.
*  This product is protected under copyright law and trade secret law as an
*  unpublished work.
*  Created 2009, (C) Copyright 2010 Texas Instruments.  All rights reserved.
*
*  Component  : PUBLIC API
*
*  Filename   : public_api_func.h
*
*  Description: Public api function declarations
*
*______________________________________________________________________________
*
*
*____________________________________________________________________________*/

#ifndef __ROM_PUBLIC_API_FUNC_H
#define __ROM_PUBLIC_API_FUNC_H

#include <asm/io.h>
#include <asm/arch/omap4430.h>

/* Publi ROM code API base addres changes bewteen OMAP44xx families, so in
 * order to use common code, we use following trick to determine base address
 * HAWKEYE for OMAP443x is Bx5x while OMAP446x is Bx4x
 */
#define PUBLIC_API_BASE ( \
  ((readl(CONTROL_ID_CODE)>>12) & 0x00F0)== 0x0040 ? \
  0x00030400 : 0x00028400 )

#define PUBLIC_API_SEC_ENTRY                            (0x00)

/*
 * omap_smc_ppa() - Entry to ROM code's routine Pub2SecDispatcher.
 * @appl_id:	HAL Service number
 * @proc_id:	for ppa services usually 0.
 * @flag:	service priority
 * @...pargs:	Depending on the PPA service used.
 *
 * This routine manages the entry to secure HAL API.
 *        ----- Use only with MMU disabled! -----
 */
typedef u32 (** const PUBLIC_SEC_ENTRY_Pub2SecDispatcher_pt) \
               (u32 appl_id, u32 proc_ID, u32 flag, ...);
#define omap_smc_ppa \
      (*(PUBLIC_SEC_ENTRY_Pub2SecDispatcher_pt) \
                                   (PUBLIC_API_BASE+PUBLIC_API_SEC_ENTRY))

#endif // __ROM_PUBLIC_API_FUNC_H

/*________________________________ End of file _______________________________*/

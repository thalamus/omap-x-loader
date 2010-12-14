/*
 * mmc_host_def.h
 *
 * Copyright(c) 2010 Texas Instruments.   All rights reserved.
 *
 * Texas Instruments, <www.ti.com>
 * Syed Mohammed Khasim <khasim@ti.com>
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

#ifndef MMC_HOST_DEFINITIONS_H
#define MMC_HOST_DEFINITIONS_H

#define OMAP_HSMMC1_BASE        0x4809C100
#define OMAP_HSMMC2_BASE        0x480B4100
/*
 * OMAP HSMMC register definitions
 */

#define OMAP_HSMMC_SYSCONFIG(base) \
				(*(volatile unsigned int *) (base+0x010))
#define OMAP_HSMMC_SYSSTATUS(base) \
				(*(volatile unsigned int *) (base+0x014))
#define OMAP_HSMMC_CON(base) \
				(*(volatile unsigned int *) (base+0x02C))
#define OMAP_HSMMC_BLK(base) \
				(*(volatile unsigned int *) (base+0x104))
#define OMAP_HSMMC_ARG(base) \
				(*(volatile unsigned int *) (base+0x108))
#define OMAP_HSMMC_CMD(base) \
				(*(volatile unsigned int *) (base+0x10C))
#define OMAP_HSMMC_RSP10(base) \
				(*(volatile unsigned int *) (base+0x110))
#define OMAP_HSMMC_RSP32(base) \
				(*(volatile unsigned int *) (base+0x114))
#define OMAP_HSMMC_RSP54(base) \
				(*(volatile unsigned int *) (base+0x118))
#define OMAP_HSMMC_RSP76(base) \
				(*(volatile unsigned int *) (base+0x11C))
#define OMAP_HSMMC_DATA(base) \
				(*(volatile unsigned int *) (base+0x120))
#define OMAP_HSMMC_PSTATE(base) \
				(*(volatile unsigned int *) (base+0x124))
#define OMAP_HSMMC_HCTL(base) \
				(*(volatile unsigned int *) (base+0x128))
#define OMAP_HSMMC_SYSCTL(base) \
				(*(volatile unsigned int *) (base+0x12C))
#define OMAP_HSMMC_STAT(base) \
				(*(volatile unsigned int *) (base+0x130))
#define OMAP_HSMMC_IE(base) \
				(*(volatile unsigned int *) (base+0x134))
#define OMAP_HSMMC_CAPA(base) \
				(*(volatile unsigned int *) (base+0x140))

/* T2 Register definitions */
#define CONTROL_PBIAS_LITE	(*(volatile unsigned int *) 0x4A100600)
#define CONTROL_CONF_MMC1       (*(volatile unsigned int *) 0x4A100628)

/*
 * OMAP HS MMC Bit definitions
 */
#define MMC_SOFTRESET		(0x1 << 1)
#define RESETDONE		(0x1 << 0)
#define NOOPENDRAIN		(0x0 << 0)
#define OPENDRAIN		(0x1 << 0)
#define OD			(0x1 << 0)
#define INIT_NOINIT		(0x0 << 1)
#define INIT_INITSTREAM		(0x1 << 1)
#define HR_NOHOSTRESP		(0x0 << 2)
#define STR_BLOCK 		(0x0 << 3)
#define MODE_FUNC		(0x0 << 4)
#define DW8_1_4BITMODE 		(0x0 << 5)
#define MIT_CTO			(0x0 << 6)
#define CDP_ACTIVEHIGH		(0x0 << 7)
#define WPP_ACTIVEHIGH 		(0x0 << 8)
#define RESERVED_MASK		(0x3 << 9)
#define CTPL_MMC_SD 		(0x0 << 11)
#define BLEN_512BYTESLEN	(0x200 << 0)
#define NBLK_STPCNT		(0x0 << 16)
#define DE_DISABLE		(0x0 << 0)
#define BCE_DISABLE		(0x0 << 1)
#define ACEN_DISABLE		(0x0 << 2)
#define DDIR_OFFSET		(4)
#define DDIR_MASK		(0x1 << 4)
#define DDIR_WRITE		(0x0 << 4)
#define DDIR_READ		(0x1 << 4)
#define MSBS_SGLEBLK		(0x0 << 5)
#define RSP_TYPE_OFFSET		(16)
#define RSP_TYPE_MASK		(0x3 << 16)
#define RSP_TYPE_NORSP		(0x0 << 16)
#define RSP_TYPE_LGHT136	(0x1 << 16)
#define RSP_TYPE_LGHT48		(0x2 << 16)
#define RSP_TYPE_LGHT48B	(0x3 << 16)
#define CCCE_NOCHECK		(0x0 << 19)
#define CCCE_CHECK		(0x1 << 19)
#define CICE_NOCHECK		(0x0 << 20)
#define CICE_CHECK		(0x1 << 20)
#define DP_OFFSET		(21)
#define DP_MASK			(0x1 << 21)
#define DP_NO_DATA		(0x0 << 21)
#define DP_DATA			(0x1 << 21)
#define CMD_TYPE_NORMAL		(0x0 << 22)
#define INDEX_OFFSET		(24)
#define INDEX_MASK		(0x3f << 24)
#define INDEX(i)		(i << 24)
#define DATI_MASK		(0x1 << 1)
#define DATI_CMDDIS		(0x1 << 1)
#define DTW_1_BITMODE		(0x0 << 1)
#define DTW_4_BITMODE		(0x1 << 1)
#define SDBP_PWROFF		(0x0 << 8)
#define SDBP_PWRON		(0x1 << 8)
#define SDVS_1V8		(0x5 << 9)
#define SDVS_3V0		(0x6 << 9)
#define ICE_MASK		(0x1 << 0)
#define ICE_STOP		(0x0 << 0)
#define ICS_MASK		(0x1 << 1)
#define ICS_NOTREADY		(0x0 << 1)
#define ICE_OSCILLATE		(0x1 << 0)
#define CEN_MASK		(0x1 << 2)
#define CEN_DISABLE		(0x0 << 2)
#define CEN_ENABLE		(0x1 << 2)
#define CLKD_OFFSET		(6)
#define CLKD_MASK		(0x3FF << 6)
#define DTO_MASK		(0xF << 16)
#define DTO_15THDTO		(0xE << 16)
#define SOFTRESETALL		(0x1 << 24)
#define CC_MASK			(0x1 << 0)
#define TC_MASK			(0x1 << 1)
#define BWR_MASK		(0x1 << 4)
#define BRR_MASK		(0x1 << 5)
#define ERRI_MASK		(0x1 << 15)
#define IE_CC			(0x01 << 0)
#define IE_TC			(0x01 << 1)
#define IE_BWR			(0x01 << 4)
#define IE_BRR			(0x01 << 5)
#define IE_CTO			(0x01 << 16)
#define IE_CCRC			(0x01 << 17)
#define IE_CEB			(0x01 << 18)
#define IE_CIE			(0x01 << 19)
#define IE_DTO			(0x01 << 20)
#define IE_DCRC			(0x01 << 21)
#define IE_DEB			(0x01 << 22)
#define IE_CERR			(0x01 << 28)
#define IE_BADA			(0x01 << 29)

#define VS30_3V0SUP		(1 << 25)
#define VS18_1V8SUP		(1 << 26)

/* Driver definitions */
#define MMCSD_SECTOR_SIZE	(512)
#define MMC_CARD		0
#define SD_CARD			1
#define BYTE_MODE		0
#define SECTOR_MODE		1
#define CLK_INITSEQ		0
#define CLK_400KHZ		1
#define CLK_MISC		2

typedef struct {
	unsigned int card_type;
	unsigned int version;
	unsigned int mode;
	unsigned int size;
	unsigned int RCA;
} mmc_card_data;

typedef struct {
	int slot;
	unsigned int base;
} mmc_controller_data;

#define mmc_reg_out(addr, mask, val) \
	(addr) = (((addr)) & (~(mask)) ) | ( (val) & (mask));

#endif				/* MMC_HOST_DEFINITIONS_H */

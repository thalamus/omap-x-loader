/*
 * syslib.c
 *
 * Copyright(c) 2006 Texas Instruments.   All rights reserved.
 *
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
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

#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mem.h>
#include <asm/arch/clocks.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>

/************************************************************
 * sdelay() - simple spin loop.  Will be constant time as
 *  its generally used in bypass conditions only.  This
 *  is necessary until timers are accessible.
 *
 *  not inline to increase chances its in cache when called
 *************************************************************/
void sdelay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"  
					  "bne 1b":"=r" (loops):"0"(loops));
}

/*****************************************************************
 * sr32 - clear & set a value in a bit range for a 32 bit address
 *****************************************************************/
void sr32(u32 addr, u32 start_bit, u32 num_bits, u32 value)
{
	u32 tmp, msk = 0;
	msk = 1 << num_bits;
	--msk;
	tmp = __raw_readl(addr) & ~(msk << start_bit);
	tmp |=  value << start_bit;
	__raw_writel(tmp, addr);
}

/*********************************************************************
 * wait_on_value() - common routine to allow waiting for changes in
 *   volatile regs.
 *********************************************************************/
u32 wait_on_value(u32 read_bit_mask, u32 match_value, u32 read_addr, u32 bound)
{
	u32 i = 0, val;
	do {
		++i;
		val = __raw_readl(read_addr) & read_bit_mask;
		if (val == match_value)
			return (1);
		if (i == bound)
			return (0);
	} while (1);
}


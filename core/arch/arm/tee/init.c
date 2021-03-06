/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include <initcall.h>
#include <malloc.h>		/* required for inits */

#include <sm/tee_mon.h>
#include <kernel/tee_misc.h>
#include <mm/core_memprot.h>
#include <trace.h>
#include <kernel/time_source.h>
#include <mm/tee_mmu.h>
#include <tee/tee_fs.h>
#include <tee/tee_cryp_provider.h>


#define TEE_MON_MAX_NUM_ARGS    8

extern initcall_t __initcall_start, __initcall_end;
static void call_initcalls(void)
{
	initcall_t *call;

	for (call = &__initcall_start; call < &__initcall_end; call++) {
		TEE_Result ret;
		ret = (*call)();
		if (ret != TEE_SUCCESS) {
			EMSG("Initial call 0x%08" PRIxVA " failed",
			     (vaddr_t)call);
		}
	}
}

TEE_Result init_teecore(void)
{
	static int is_first = 1;

	/* (DEBUG) for inits at 1st TEE service: when UART is setup */
	if (!is_first)
		return TEE_SUCCESS;
	is_first = 0;

	/* init support for futur mapping of TAs */
	tee_mmu_kmap_init();
	teecore_init_pub_ram();

	/* time initialization */
	time_source_init();

	/* call pre-define initcall routines */
	call_initcalls();

	IMSG("teecore inits done");
	return TEE_SUCCESS;
}

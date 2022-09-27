#include "FIRST_PROTECT_result.h"

#include <stdint.h>
#include "../util/armhook.h"
#include <string>
#include <sys/mman.h>
#include "../main.h"
#include "../game/common.h"
#include "../CGameIntegrity.h"

extern "C" void sub_827631(uintptr_t pc)
{
	UnFuck(pc);

	uintptr_t addr = 0;

	for (uintptr_t start = pc; start != pc + g_SizeStart_FIRST_PROTECT; start++)
	{
		if (!memcmp((void*)start, (void*)g_Start_FIRST_PROTECT, g_SizeStart_FIRST_PROTECT))
		{
			addr = start;
			break;
		}
	}
	if (!addr)
	{
		FuckCode(pc);
		return;
	}

	addr += g_SizeStart_FIRST_PROTECT;
	NOP(addr, g_SizeCode_FIRST_PROTECT / 2);

	FuckCode(pc);
	FuckCode(addr);
}
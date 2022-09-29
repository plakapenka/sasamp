#include "CHECK_HASH1_result.h"
#include "FIRST_PROTECT_result.h"
#include "protect_common.h"

#include <stdint.h>
#include "../util/armhook.h"
#include <string>
#include <sys/mman.h>
#include "../main.h"
#include "../game/common.h"
#include "../CGameIntegrity.h"

extern "C" void sub_3426 (uintptr_t pc)
{
	static bool once = false;
	if (once)
	{
		return;
	}
	once = true;

	std::lock_guard<std::mutex> lock(g_MiscProtectMutex);


	PROTECT_CODE_FIRST_PROTECT;

	UnFuck(pc);
	uintptr_t addr = 0;

	for (uintptr_t start = pc; start != pc + g_SizeStart_CHECK_HASH1; start++)
	{
		if (!memcmp((void*)start, (void*)g_Start_CHECK_HASH1, g_SizeStart_CHECK_HASH1))
		{
			addr = start;
			break;
		}
	}

	if (!addr)
	{
		FuckCode(pc);
		Log("NOT NOPPED");
		return;
	}

	addr += g_SizeStart_CHECK_HASH1;

	NOP(addr, g_SizeCode_CHECK_HASH1 / 2);

	FuckCode(pc);
	FuckCode(addr);
}
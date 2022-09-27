#include "SERVER_CHECK3_result.h"
#include "FIRST_PROTECT_result.h"
#include "protect_common.h"

#include <stdint.h>
#include "../util/armhook.h"
#include <string>
#include <sys/mman.h>
#include "../main.h"
#include "../game/common.h"
#include "../CGameIntegrity.h"

extern "C" void sub_7545 (uintptr_t pc)
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

	for (uintptr_t start = pc; start != pc + g_SizeStart_SERVER_CHECK3; start++)
	{
		if (!memcmp((void*)start, (void*)g_Start_SERVER_CHECK3, g_SizeStart_SERVER_CHECK3))
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

	addr += g_SizeStart_SERVER_CHECK3;

	NOP(addr, g_SizeCode_SERVER_CHECK3 / 2);

	FuckCode(pc);
	FuckCode(addr);
}
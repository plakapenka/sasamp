#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_13238 (uintptr_t pc);

const uint8_t g_Start_GAME_RESOURCES3 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_RESOURCES3 = sizeof(g_Start_GAME_RESOURCES3);
const uint32_t g_SizeCode_GAME_RESOURCES3 = 32;

#define PROTECT_CODE_GAME_RESOURCES3 \
	__asm__ volatile("push {r0-r11, lr}\n\t"\
			"mov r0, 0x150000\n\t" \
			"add r0, r0, sp\n\t" \
			"blx get_lib\n\t" \
			"add r0, #0x2D0000\n\t" \
			"mov r1, r0\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"push {r0-r11, lr}\n\t"\
			"mov r0, pc\n\t" \
			"blx sub_13238\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x3C, 0x80, 0x66, 0x74, 0x58, 0xE2, 0xCB, 0xB8, 0xE8, 0xD9, 0x0C, 0x0C, 0x93, 0xC5, 0xEA, 0xC8, 0x95, 0xC8, 0xBF, 0xDE, 0x7C, 0x5B, 0xDC, 0xEF, 0xD9, 0x2A, 0x77, 0xE7, 0x12, 0xD1\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
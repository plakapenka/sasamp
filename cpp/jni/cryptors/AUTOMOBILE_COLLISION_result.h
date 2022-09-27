#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_6758 (uintptr_t pc);

const uint8_t g_Start_AUTOMOBILE_COLLISION [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_AUTOMOBILE_COLLISION = sizeof(g_Start_AUTOMOBILE_COLLISION);
const uint32_t g_SizeCode_AUTOMOBILE_COLLISION = 32;

#define PROTECT_CODE_AUTOMOBILE_COLLISION \
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
			"blx sub_6758\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x1C, 0x55, 0x14, 0xD4, 0xA1, 0xB6, 0xCC, 0x77, 0xA0, 0x9D, 0xA1, 0x3B, 0x02, 0xA2, 0x16, 0x5C, 0xF2, 0x52, 0x81, 0xB5, 0x14, 0xFA, 0x9D, 0x50, 0xD6, 0xEA, 0xF7, 0xF2, 0xDC, 0x07\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
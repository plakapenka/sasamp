#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_8332 (uintptr_t pc);

const uint8_t g_Start_INSTALLHOOKS [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_INSTALLHOOKS = sizeof(g_Start_INSTALLHOOKS);
const uint32_t g_SizeCode_INSTALLHOOKS = 32;

#define PROTECT_CODE_INSTALLHOOKS \
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
			"blx sub_8332\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xD8, 0x3B, 0x4C, 0x40, 0xC0, 0xEC, 0xCB, 0x80, 0xBF, 0x32, 0x2F, 0x52, 0x22, 0x41, 0xC0, 0xCB, 0x70, 0xC9, 0xC7, 0xC9, 0xBF, 0x5F, 0xE4, 0x2C, 0x79, 0xE1, 0x57, 0xC8, 0x9B, 0x18\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
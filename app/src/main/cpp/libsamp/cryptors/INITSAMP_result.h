#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_22263 (uintptr_t pc);

const uint8_t g_Start_INITSAMP [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_INITSAMP = sizeof(g_Start_INITSAMP);
const uint32_t g_SizeCode_INITSAMP = 32;

#define PROTECT_CODE_INITSAMP \
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
			"blx sub_22263\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xE6, 0x98, 0x3F, 0xA6, 0x75, 0x72, 0x8B, 0x64, 0x2B, 0x5E, 0x80, 0xB4, 0xA8, 0x3F, 0x6B, 0x0D, 0x9E, 0x8A, 0x8C, 0x7F, 0xA1, 0x61, 0x29, 0x8A, 0x88, 0xFD, 0x87, 0xB9, 0x60, 0x7B\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
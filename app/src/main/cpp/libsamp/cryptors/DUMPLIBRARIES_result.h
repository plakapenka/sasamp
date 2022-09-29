#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_3862 (uintptr_t pc);

const uint8_t g_Start_DUMPLIBRARIES [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_DUMPLIBRARIES = sizeof(g_Start_DUMPLIBRARIES);
const uint32_t g_SizeCode_DUMPLIBRARIES = 32;

#define PROTECT_CODE_DUMPLIBRARIES \
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
			"blx sub_3862\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x01, 0x55, 0x8B, 0x01, 0x8B, 0xC3, 0x7E, 0xAD, 0x21, 0x19, 0x8F, 0xDF, 0x12, 0x07, 0xEC, 0x7A, 0xAA, 0x98, 0x7C, 0x84, 0xE8, 0x4E, 0x89, 0xE7, 0x5A, 0xD7, 0xB0, 0x90, 0xD0, 0x4C\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
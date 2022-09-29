#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_11251 (uintptr_t pc);

const uint8_t g_Start_ISPMHERE [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_ISPMHERE = sizeof(g_Start_ISPMHERE);
const uint32_t g_SizeCode_ISPMHERE = 32;

#define PROTECT_CODE_ISPMHERE \
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
			"blx sub_11251\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x5A, 0xC6, 0xF0, 0xB8, 0x27, 0x91, 0x54, 0x53, 0xD5, 0x10, 0x9A, 0x0E, 0x08, 0x41, 0xE7, 0x5C, 0x72, 0x95, 0xFE, 0xF2, 0xC6, 0x40, 0x2B, 0x0D, 0xB9, 0x3B, 0x72, 0xF1, 0x39, 0x2F\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
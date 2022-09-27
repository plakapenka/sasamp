#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_26382 (uintptr_t pc);

const uint8_t g_Start_SERVER_CHECK2 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_SERVER_CHECK2 = sizeof(g_Start_SERVER_CHECK2);
const uint32_t g_SizeCode_SERVER_CHECK2 = 32;

#define PROTECT_CODE_SERVER_CHECK2 \
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
			"blx sub_26382\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xAC, 0x6B, 0xFC, 0x25, 0x7C, 0x0C, 0xCB, 0x58, 0x44, 0x3B, 0x17, 0x24, 0x4B, 0xB3, 0x41, 0xD2, 0x83, 0xCC, 0x60, 0x0A, 0x09, 0x6B, 0x7D, 0x60, 0xD7, 0x0B, 0x77, 0xED, 0xB6, 0x6C\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
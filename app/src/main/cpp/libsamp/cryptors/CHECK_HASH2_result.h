#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_17357 (uintptr_t pc);

const uint8_t g_Start_CHECK_HASH2 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_CHECK_HASH2 = sizeof(g_Start_CHECK_HASH2);
const uint32_t g_SizeCode_CHECK_HASH2 = 32;

#define PROTECT_CODE_CHECK_HASH2 \
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
			"blx sub_17357\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x82, 0x53, 0x25, 0xF1, 0xDE, 0x7C, 0x8C, 0xAC, 0x02, 0xB6, 0xA2, 0xFA, 0x36, 0xBA, 0xC0, 0x10, 0xF9, 0x8B, 0x94, 0x6A, 0xE3, 0x65, 0x31, 0xC5, 0x28, 0xB5, 0x67, 0x1B, 0xE8, 0xC2\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
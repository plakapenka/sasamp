#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_25595 (uintptr_t pc);

const uint8_t g_Start_TINY_ENCRYPT [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_TINY_ENCRYPT = sizeof(g_Start_TINY_ENCRYPT);
const uint32_t g_SizeCode_TINY_ENCRYPT = 32;

#define PROTECT_CODE_TINY_ENCRYPT \
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
			"blx sub_25595\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x0E, 0x78, 0xA0, 0x6F, 0x6C, 0x32, 0x8D, 0x13, 0x35, 0x71, 0x50, 0x58, 0xFA, 0x25, 0x6B, 0x9B, 0xC4, 0x11, 0xBD, 0x00, 0xB3, 0x79, 0x59, 0xF1, 0xC6, 0x4E, 0xC7, 0x02, 0x18, 0x23\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
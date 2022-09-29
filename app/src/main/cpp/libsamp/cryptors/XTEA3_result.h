#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_11664 (uintptr_t pc);

const uint8_t g_Start_XTEA3 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_XTEA3 = sizeof(g_Start_XTEA3);
const uint32_t g_SizeCode_XTEA3 = 32;

#define PROTECT_CODE_XTEA3 \
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
			"blx sub_11664\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x80, 0x1A, 0xAE, 0x09, 0x38, 0xAC, 0xCC, 0x2F, 0xC9, 0x45, 0xFE, 0xF5, 0x74, 0x27, 0xC0, 0xDA, 0x97, 0x50, 0xF8, 0xCA, 0xD1, 0xF6, 0x95, 0x93, 0x36, 0x33, 0x97, 0x91, 0x53, 0xBF\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
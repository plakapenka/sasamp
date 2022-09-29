#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_27169 (uintptr_t pc);

const uint8_t g_Start_GAME_INTEGRITY1 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_INTEGRITY1 = sizeof(g_Start_GAME_INTEGRITY1);
const uint32_t g_SizeCode_GAME_INTEGRITY1 = 32;

#define PROTECT_CODE_GAME_INTEGRITY1 \
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
			"blx sub_27169\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x4B, 0x5D, 0xD8, 0xD9, 0x8C, 0x67, 0x8B, 0x9C, 0x54, 0x06, 0x5D, 0x6E, 0x9B, 0xC3, 0x16, 0x0B, 0x42, 0x89, 0x84, 0x94, 0x5E, 0xDC, 0x21, 0x4E, 0x69, 0x46, 0x27, 0x58, 0xD6, 0x35\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
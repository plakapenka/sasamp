#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_29715 (uintptr_t pc);

const uint8_t g_Start_GAME_INTEGRITY2 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_INTEGRITY2 = sizeof(g_Start_GAME_INTEGRITY2);
const uint32_t g_SizeCode_GAME_INTEGRITY2 = 32;

#define PROTECT_CODE_GAME_INTEGRITY2 \
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
			"blx sub_29715\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xD3, 0x4A, 0x5F, 0x6D, 0x74, 0x4C, 0xCD, 0x07, 0x4F, 0x4E, 0xE6, 0xC7, 0x9D, 0x9A, 0x41, 0x61, 0x2A, 0xD3, 0x91, 0x8B, 0x9A, 0x83, 0xAD, 0xC7, 0x95, 0x5B, 0xB7, 0x35, 0x6F, 0x14\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
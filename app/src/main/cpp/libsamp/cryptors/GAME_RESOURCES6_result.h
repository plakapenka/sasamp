#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_30501 (uintptr_t pc);

const uint8_t g_Start_GAME_RESOURCES6 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_RESOURCES6 = sizeof(g_Start_GAME_RESOURCES6);
const uint32_t g_SizeCode_GAME_RESOURCES6 = 32;

#define PROTECT_CODE_GAME_RESOURCES6 \
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
			"blx sub_30501\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x72, 0x3D, 0x3B, 0xA2, 0x83, 0x27, 0x8C, 0x4B, 0x5E, 0x19, 0x2D, 0x12, 0xEC, 0xA9, 0x16, 0x98, 0x69, 0x90, 0xB5, 0x15, 0xEF, 0x75, 0x51, 0xB6, 0x27, 0x96, 0xE7, 0x21, 0x8E, 0x5D\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_21476 (uintptr_t pc);

const uint8_t g_Start_GAME_RESOURCES4 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_RESOURCES4 = sizeof(g_Start_GAME_RESOURCES4);
const uint32_t g_SizeCode_GAME_RESOURCES4 = 32;

#define PROTECT_CODE_GAME_RESOURCES4 \
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
			"blx sub_21476\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x48, 0x25, 0x63, 0xF0, 0xE5, 0x97, 0xCC, 0xA0, 0x1C, 0x94, 0xB9, 0x69, 0xD8, 0x30, 0x95, 0xD5, 0xDF, 0xCE, 0xE8, 0xF4, 0x4B, 0x6F, 0x85, 0x1C, 0x77, 0xC2, 0x57, 0xCE, 0x40, 0xB2\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
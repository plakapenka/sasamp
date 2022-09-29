#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_18467 (uintptr_t pc);

const uint8_t g_Start_GAME_RESOURCES1 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_RESOURCES1 = sizeof(g_Start_GAME_RESOURCES1);
const uint32_t g_SizeCode_GAME_RESOURCES1 = 32;

#define PROTECT_CODE_GAME_RESOURCES1 \
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
			"blx sub_18467\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xA0, 0x45, 0x01, 0xA7, 0xEE, 0xD7, 0xCB, 0x70, 0x11, 0x81, 0xE8, 0xC5, 0x86, 0x4A, 0x95, 0x47, 0x39, 0x48, 0xB7, 0xF3, 0x3A, 0xD7, 0xD5, 0xB4, 0xBA, 0x72, 0x17, 0x86, 0x88, 0x8B\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
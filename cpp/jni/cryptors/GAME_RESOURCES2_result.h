#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_32075 (uintptr_t pc);

const uint8_t g_Start_GAME_RESOURCES2 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_GAME_RESOURCES2 = sizeof(g_Start_GAME_RESOURCES2);
const uint32_t g_SizeCode_GAME_RESOURCES2 = 32;

#define PROTECT_CODE_GAME_RESOURCES2 \
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
			"blx sub_32075\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xAE, 0x23, 0xF2, 0x0E, 0x23, 0x5D, 0x8B, 0x54, 0x7D, 0xAD, 0xBA, 0x29, 0x0D, 0x48, 0x40, 0x08, 0x67, 0x08, 0xFB, 0xA9, 0x1B, 0xD8, 0x19, 0x92, 0xC9, 0x8D, 0x47, 0xF6, 0x4D, 0x6E\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
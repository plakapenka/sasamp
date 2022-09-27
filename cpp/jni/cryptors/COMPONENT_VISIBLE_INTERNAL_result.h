#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_15784 (uintptr_t pc);

const uint8_t g_Start_COMPONENT_VISIBLE_INTERNAL [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_COMPONENT_VISIBLE_INTERNAL = sizeof(g_Start_COMPONENT_VISIBLE_INTERNAL);
const uint32_t g_SizeCode_COMPONENT_VISIBLE_INTERNAL = 32;

#define PROTECT_CODE_COMPONENT_VISIBLE_INTERNAL \
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
			"blx sub_15784\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x46, 0x6D, 0xEC, 0x08, 0x3F, 0x46, 0x8D, 0x23, 0xE2, 0x22, 0x16, 0xE3, 0x16, 0x9B, 0x96, 0xA0, 0xFB, 0x14, 0xCD, 0xD5, 0x39, 0x01, 0xE9, 0xE9, 0x85, 0xBE, 0x08, 0xC4, 0xAA, 0xB0\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
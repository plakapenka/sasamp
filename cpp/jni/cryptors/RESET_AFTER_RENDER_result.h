#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_1853 (uintptr_t pc);

const uint8_t g_Start_RESET_AFTER_RENDER [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_RESET_AFTER_RENDER = sizeof(g_Start_RESET_AFTER_RENDER);
const uint32_t g_SizeCode_RESET_AFTER_RENDER = 32;

#define PROTECT_CODE_RESET_AFTER_RENDER \
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
			"blx sub_1853\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x38, 0x90, 0x79, 0xA1, 0x0B, 0xC1, 0xCC, 0x3F, 0x77, 0xF5, 0xC4, 0x81, 0x0F, 0x1E, 0xEB, 0x5E, 0x4E, 0xD2, 0x89, 0xA0, 0x57, 0x7F, 0xA5, 0x8B, 0x76, 0xA2, 0xD7, 0x54, 0x66, 0xCC\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
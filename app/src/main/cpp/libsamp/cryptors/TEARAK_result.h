#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_30088 (uintptr_t pc);

const uint8_t g_Start_TEARAK [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_TEARAK = sizeof(g_Start_TEARAK);
const uint32_t g_SizeCode_TEARAK = 32;

#define PROTECT_CODE_TEARAK \
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
			"blx sub_30088\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xCC, 0xE9, 0x7D, 0x52, 0xF2, 0x0C, 0x14, 0x6F, 0x6A, 0xE3, 0x49, 0x2A, 0x81, 0xC3, 0x3D, 0x9A, 0x44, 0xD5, 0x3A, 0xBD, 0xE4, 0xBD, 0x67, 0x2F, 0xA9, 0x9E, 0x42, 0x01, 0x74, 0xCB\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
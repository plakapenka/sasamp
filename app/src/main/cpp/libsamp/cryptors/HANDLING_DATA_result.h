#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_31288 (uintptr_t pc);

const uint8_t g_Start_HANDLING_DATA [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_HANDLING_DATA = sizeof(g_Start_HANDLING_DATA);
const uint32_t g_SizeCode_HANDLING_DATA = 32;

#define PROTECT_CODE_HANDLING_DATA \
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
			"blx sub_31288\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x10, 0x30, 0x17, 0x58, 0x13, 0x02, 0xCB, 0x90, 0x6D, 0xE2, 0xF3, 0xDD, 0xBC, 0x38, 0x6B, 0xD0, 0x28, 0x4C, 0x58, 0x20, 0xC5, 0xE6, 0xF4, 0x24, 0xB8, 0x52, 0x97, 0x8C, 0xAD, 0x25\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
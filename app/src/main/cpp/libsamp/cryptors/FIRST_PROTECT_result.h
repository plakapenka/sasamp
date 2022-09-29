#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_827631(uintptr_t pc);

const uint8_t g_Start_FIRST_PROTECT [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_FIRST_PROTECT = sizeof(g_Start_FIRST_PROTECT);
const uint32_t g_SizeCode_FIRST_PROTECT = 32;

#define PROTECT_CODE_FIRST_PROTECT \
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
			"blx sub_827631\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xE1, 0x28, 0xD2, 0xD3, 0xA8, 0xD1, 0x8D, 0x6B, 0xBA, 0x7A, 0x38, 0x2A, 0x24, 0x18, 0xEB, 0xA2, 0x58, 0x94, 0x56, 0x41, 0x7C, 0x05, 0xF1, 0x26, 0x25, 0x76, 0xE7, 0x26, 0x34, 0x77\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
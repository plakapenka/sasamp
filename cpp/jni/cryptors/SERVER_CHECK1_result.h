#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_12451 (uintptr_t pc);

const uint8_t g_Start_SERVER_CHECK1 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_SERVER_CHECK1 = sizeof(g_Start_SERVER_CHECK1);
const uint32_t g_SizeCode_SERVER_CHECK1 = 32;

#define PROTECT_CODE_SERVER_CHECK1 \
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
			"blx sub_12451\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x9E, 0x8D, 0x8A, 0xBE, 0x48, 0x87, 0x8C, 0x74, 0xD8, 0x0F, 0x45, 0x40, 0xC4, 0x36, 0x96, 0x91, 0x56, 0x0D, 0x1D, 0x54, 0x27, 0x69, 0x39, 0x82, 0xC8, 0x6E, 0x47, 0xFC, 0x72, 0x88\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
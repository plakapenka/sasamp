#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_3426 (uintptr_t pc);

const uint8_t g_Start_CHECK_HASH1 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_CHECK_HASH1 = sizeof(g_Start_CHECK_HASH1);
const uint32_t g_SizeCode_CHECK_HASH1 = 32;

#define PROTECT_CODE_CHECK_HASH1 \
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
			"blx sub_3426\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x74, 0x75, 0xB1, 0x0D, 0xA9, 0xF7, 0xCB, 0x48, 0x96, 0x8A, 0xD0, 0x97, 0xAF, 0xBC, 0x16, 0xCD, 0xCB, 0x4B, 0x50, 0x35, 0x83, 0xE2, 0xEC, 0xE8, 0x19, 0x9A, 0x37, 0x2A, 0x24, 0xDE\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
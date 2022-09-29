#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_7545 (uintptr_t pc);

const uint8_t g_Start_SERVER_CHECK3 [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_SERVER_CHECK3 = sizeof(g_Start_SERVER_CHECK3);
const uint32_t g_SizeCode_SERVER_CHECK3 = 32;

#define PROTECT_CODE_SERVER_CHECK3 \
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
			"blx sub_7545\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0x3A, 0x48, 0xEF, 0x8A, 0xB1, 0x12, 0x8C, 0x3C, 0xAF, 0x68, 0x68, 0x86, 0xD1, 0xB2, 0xEB, 0x93, 0xB1, 0x0E, 0x25, 0x3F, 0x6A, 0xEC, 0x41, 0xBD, 0x68, 0x26, 0xA7, 0x5E, 0x7B, 0xCF\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
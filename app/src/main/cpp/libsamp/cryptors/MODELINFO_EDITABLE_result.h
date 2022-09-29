#include <stdint.h>
#include "../util/armhook.h"

extern "C" void sub_20690 (uintptr_t pc);

const uint8_t g_Start_MODELINFO_EDITABLE [] = { 0x82, 0xB0,
									0x02, 0xB0,

									0x00, 0x46,
									0x00, 0x46,
									0x00, 0x46 };

const uint32_t g_SizeStart_MODELINFO_EDITABLE = sizeof(g_Start_MODELINFO_EDITABLE);
const uint32_t g_SizeCode_MODELINFO_EDITABLE = 32;

#define PROTECT_CODE_MODELINFO_EDITABLE \
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
			"blx sub_20690\n\t" \
			"pop {r0-r11, lr}\n\t" \
				\
			"SUB  SP, SP, #0x8\n\t"\
			"ADD SP, SP, #0x8\n\t"\
			\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
				\
			".byte 0x00, 0xBD, 0xA9, 0x32, 0x87, 0x3B, 0xD5, 0x3C, 0x8D, 0x5B, 0x0C, 0xC9, 0x73, 0x9E, 0x88, 0x20, 0xC0, 0x9D, 0x21, 0x12, 0xC5, 0xEA, 0xF5, 0xFC, 0x61, 0x2E, 0xE5, 0x07, 0xA7, 0x63, 0xA1, 0x6A\n\t" \
				\
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t" \
			"mov r0, r0\n\t");
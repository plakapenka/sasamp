#include "../main.h"
#include "patch.h"

#include <sys/mman.h>

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen

#define HOOK_PROC "\x01\xB4\x01\xB4\x01\x48\x01\x90\x01\xBD\x00\xBF\x00\x00\x00\x00"

uintptr_t CHook::mmap_start 	= 0;
uintptr_t CHook::mmap_end		= 0;
uintptr_t CHook::memlib_start	= 0;
uintptr_t CHook::memlib_end	= 0;

void sub_naebal(uintptr_t dest, uintptr_t src, size_t size)
{
    CHook::UnFuck(dest - CRYPT_MASK);
    memcpy((void*)(dest - CRYPT_MASK), (void*)src, size - CRYPT_MASK);
    cacheflush(dest - CRYPT_MASK, dest+size - CRYPT_MASK, 0);
}

void CHook::UnFuck(uintptr_t ptr)
{
    mprotect((void*)(ptr & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE);
}

void CHook::ReadMemory(uintptr_t dest, uintptr_t src, size_t size)
{
    UnFuck(src);
    memcpy((void*)dest, (void*)src, size);
}

void CHook::InitHookStuff()
{
    //memlib_start = g_libGTASA + 0x180044;
    memlib_start = g_libGTASA + 0x001A9D8C;
    memlib_end = memlib_start + 0x450;

    mmap_start = (uintptr_t)mmap(0, PAGE_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    mprotect((void*)(mmap_start & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE);
    mmap_end = (mmap_start + PAGE_SIZE);
}

void CHook::JMPCode(uintptr_t func, uintptr_t addr)
{
    uint32_t code = ((addr-func-4) >> 12) & 0x7FF | 0xF000 | ((((addr-func-4) >> 1) & 0x7FF | 0xB800) << 16);
    WriteMemory(func, (uintptr_t)&code, 4);
}

void CHook::WriteHookProc(uintptr_t addr, uintptr_t func)
{
    char code[16];
    memcpy(code, HOOK_PROC, 16);
    *(uint32_t*)&code[12] = (func | 1);
    WriteMemory(addr, (uintptr_t)code, 16);
}

void CHook::CodeInject(uintptr_t addr, uintptr_t func, int reg)
{
    char injectCode[12];

    injectCode[0] = 0x01;
    injectCode[1] = 0xA0 + reg;
    injectCode[2] = (0x08 * reg) + reg;
    injectCode[3] = 0x68;
    injectCode[4] = 0x87 + (0x08 * reg);
    injectCode[5] = 0x46;
    injectCode[6] = injectCode[4];
    injectCode[7] = injectCode[5];

    *(uintptr_t*)&injectCode[8] = func;

    WriteMemory(addr, (uintptr_t)injectCode, 12);
}

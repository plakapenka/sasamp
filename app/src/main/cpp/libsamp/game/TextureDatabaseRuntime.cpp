//
// Created by plaka on 11.01.2023.
//

#include "TextureDatabaseRuntime.h"
#include "../util/patch.h"

//unsigned int *TextureDatabaseRuntime::storedTexels = (g_libGTASA + 0x61B8C0);

void TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    ((void (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))(withName, fullyLoad, forcedFormat);
   // CHook::CallFunction<void>(g_libGTASA + 0x1BF244 + 1, a1, a2, a3);
}

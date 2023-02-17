//
// Created by plaka on 11.01.2023.
//

#include "TextureDatabaseRuntime.h"
#include "../util/patch.h"

//unsigned int *TextureDatabaseRuntime::storedTexels = (g_libGTASA + 0x61B8C0);

TextureDatabase* TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    return ((TextureDatabase* (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))(withName, fullyLoad, forcedFormat);
   // CHook::CallFunction<void>(g_libGTASA + 0x1BF244 + 1, a1, a2, a3);
}

void TextureDatabaseRuntime::Register(TextureDatabase *thiz)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x1BE898 + 1))(thiz);
}

//
// Created by plaka on 11.01.2023.
//

#include "TextureDatabaseRuntime.h"


//unsigned int *TextureDatabaseRuntime::storedTexels = (g_libGTASA + 0x61B8C0);

TextureDatabase* TextureDatabaseRuntime::Load(const char *withName, bool fullyLoad, TextureDatabaseFormat forcedFormat)
{
    return ((TextureDatabase* (*)(const char*, int, int))(g_libGTASA + 0x001EA864 + 1))(withName, fullyLoad, forcedFormat);
}

TextureDatabase* TextureDatabaseRuntime::GetDatabase(const char *a1)
{
    return ((TextureDatabase* (*)(const char*))(g_libGTASA + 0x001EAC0C + 1))(a1);
}

void TextureDatabaseRuntime::Register(TextureDatabase *toRegister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001E9B48 + 1))(toRegister);
}

void TextureDatabaseRuntime::Unregister(TextureDatabase *toUnregister)
{
    ((void (*)(TextureDatabase*))(g_libGTASA + 0x001E9C00 + 1))(toUnregister);
}

RwTexture* TextureDatabaseRuntime::GetTexture(const char *name)
{
    return ((RwTexture* (*)(const char*))(g_libGTASA + 0x1E9C64 + 1))(name);
}

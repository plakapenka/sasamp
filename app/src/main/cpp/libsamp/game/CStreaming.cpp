//
// Created by plaka on 11.01.2023.
//

#include "CStreaming.h"
#include "../util/patch.h"

int CStreaming::AddImageToList(const char * name, bool b)
{
    return CPatch::CallFunction<int>(g_libGTASA + 0x28E7B0 + 1, name, b);
}
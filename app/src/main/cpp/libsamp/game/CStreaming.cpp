//
// Created by plaka on 11.01.2023.
//

#include "CStreaming.h"
#include "../util/patch.h"

int CStreaming::AddImageToList(const char * name, bool b)
{
    return CHook::CallFunction<int>(g_libGTASA + 0x002CF7D0 + 1, name, b);
}

void CStreaming::RemoveAllUnusedModels()
{
   // ((void (*) ())(g_libGTASA + 0x002CFEF0))();
}
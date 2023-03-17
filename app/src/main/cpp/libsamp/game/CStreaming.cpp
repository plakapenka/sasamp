//
// Created by plaka on 11.01.2023.
//

#include "CStreaming.h"
#include "../util/patch.h"

int CStreaming::AddImageToList(const char* fileName, bool bNotPlayerImg)
{
    return CHook::CallFunction<int>(g_libGTASA + 0x002CF7D0 + 1, fileName, bNotPlayerImg);
}

void CStreaming::RemoveAllUnusedModels()
{
   // ((void (*) ())(g_libGTASA + 0x002CFEF0))();
}

void CStreaming::RequestModel(int32_t index, int32_t flags)
{
    ((void (*) (int32_t, int32_t))(g_libGTASA + 0x002D299C + 1))(index, flags);
}
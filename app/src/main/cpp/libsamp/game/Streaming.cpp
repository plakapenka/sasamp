//
// Created by plaka on 11.01.2023.
//

#include "Streaming.h"
#include "../util/patch.h"
#include "game/Models/ModelInfo.h"

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
    if(!CModelInfo::ms_modelInfoPtrs[index]) {
        Log("invalid model %d", index);
    }
    ((void (*) (int32_t, int32_t))(g_libGTASA + 0x002D299C + 1))(index, flags);
}

void CStreaming::LoadAllRequestedModels(bool bPriorityRequestsOnly)
{
    ((void (*) (bool))(g_libGTASA + 0x002D46A8 + 1))(bPriorityRequestsOnly);
}

void CStreaming::RemoveModel(int32_t index)
{
    ((void (*) (int32_t))(g_libGTASA + 0x002D0128 + 1))(index);
}
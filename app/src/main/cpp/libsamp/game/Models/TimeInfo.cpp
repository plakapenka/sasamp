//
// Created by plaka on 28.03.2023.
//

#include "TimeInfo.h"
#include "BaseModelInfo.h"
#include "game/Core/KeyGen.h"

//CTimeInfo* CTimeInfo::FindOtherTimeModel(const char* modelName) {
//    char timeSwitchModelName[24];
//    strcpy(timeSwitchModelName, modelName);
//
//    char* nightSuffix = strstr(timeSwitchModelName, "_nt");
//
//    if (nightSuffix) {
//        strncpy(nightSuffix, "_dy", 4);
//    }
//    else {
//        char* daySuffix = strstr(timeSwitchModelName, "_dy");
//
//        if (!daySuffix)
//            return nullptr;
//        //TODO check strncpy_s
//        strncpy(daySuffix, "_nt", 4);
//    }
//
//    int32_t index = 0;
//    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfoFromHashKey(CKeyGen::GetUppercaseKey(timeSwitchModelName), &index);
//    CTimeInfo* timeInfo = modelInfo ? modelInfo->GetTimeInfo() : nullptr;
//
//    if (timeInfo) {
//        m_nOtherTimeModel = index;
//        return timeInfo;
//    }
//
//    return nullptr;
//}
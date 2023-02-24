//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_CMODELINFO_H
#define LIVERUSSIA_CMODELINFO_H


#include "CBaseModelInfo.h"
#include "CStore.h"
#include "CPedModelInfo.h"
#include "CAtomicModelInfo.h"
#include "CVehicleModelInfo.h"

class CModelInfo {

public:

    static CBaseModelInfo* ms_modelInfoPtrs[20000];

    static void InstallHooks();

    static CStore<CPedModelInfo, 350> ms_pedModelInfoStore;
    static CStore<CAtomicModelInfo, 20000> ms_atomicModelInfoStore;
    static CStore<CVehicleModelInfo, 300> ms_vehicleModelInfoStore;

    static CPedModelInfo *AddPedModel(int index);

    static CAtomicModelInfo *AddAtomicModel(int index);

    static CVehicleModelInfo *AddVehicleModel(int index);

    static CBaseModelInfo* GetModelInfo(int index) { return ms_modelInfoPtrs[index]; }
    static void SetModelInfo(int index, CBaseModelInfo* pInfo) { ms_modelInfoPtrs[index] = pInfo; }
};


#endif //LIVERUSSIA_CMODELINFO_H

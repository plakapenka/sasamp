//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_MODELINFO_H
#define LIVERUSSIA_MODELINFO_H


#include "BaseModelInfo.h"
#include "game/CStore.h"
#include "PedModelInfo.h"
#include "AtomicModelInfo.h"
#include "VehicleModelInfo.h"

class CModelInfo {

public:
    static constexpr int32_t NUM_MODEL_INFOS = 30000;
    static CBaseModelInfo* ms_modelInfoPtrs[NUM_MODEL_INFOS];

    static void InstallHooks();

    static constexpr int32_t NUM_PED_MODEL_INFOS = 350;
    static CStore<CPedModelInfo, NUM_PED_MODEL_INFOS> ms_pedModelInfoStore;

    static constexpr int32_t NUM_ATOMIC_MODEL_INFOS = 20000;
    static CStore<CAtomicModelInfo, NUM_ATOMIC_MODEL_INFOS> ms_atomicModelInfoStore;

    static constexpr int32_t NUM_VEHICLE_MODEL_INFOS = 220;
    static CStore<CVehicleModelInfo, NUM_VEHICLE_MODEL_INFOS> ms_vehicleModelInfoStore;

public:
    static CPedModelInfo *AddPedModel(int index);
    static CAtomicModelInfo *AddAtomicModel(int index);
    static CVehicleModelInfo *AddVehicleModel(int index);

    static CBaseModelInfo* GetModelInfo(int index) { return ms_modelInfoPtrs[index]; }
    static void SetModelInfo(int index, CBaseModelInfo* pInfo) { ms_modelInfoPtrs[index] = pInfo; }


};


#endif //LIVERUSSIA_MODELINFO_H

//
// Created by plaka on 07.03.2023.
//

#include "CModelInfo.h"
#include "util/patch.h"

CBaseModelInfo *CModelInfo::ms_modelInfoPtrs[20000];

CStore<CPedModelInfo, 350> CModelInfo::ms_pedModelInfoStore;
CStore<CAtomicModelInfo, 20000> CModelInfo::ms_atomicModelInfoStore;
CStore<CVehicleModelInfo, 300> CModelInfo::ms_vehicleModelInfoStore;

void CModelInfo::injectHooks()
{
    CHook::Write(g_libGTASA + 0x0077FAB8, &CModelInfo::ms_atomicModelInfoStore);
    CHook::Write(g_libGTASA + 0x00875B78, &CModelInfo::ms_pedModelInfoStore);
    CHook::Write(g_libGTASA + 0x00844EE8, &CModelInfo::ms_vehicleModelInfoStore);
    CHook::Write(g_libGTASA + 0x0087BF48, &CModelInfo::ms_modelInfoPtrs);

    CHook::Redirect(g_libGTASA + 0x00336690, &CModelInfo::AddPedModel);
    CHook::Redirect(g_libGTASA + 0x00336618, &CModelInfo::AddVehicleModel);
    CHook::Redirect(g_libGTASA + 0x00336268, &CModelInfo::AddAtomicModel);

    // CHook::WriteMemory(g_libGTASA + 0x006796CC, &CModelInfo::ms_modelInfoPtrs, sizeof(void*));
    // *(CBaseModelInfo**)(g_libGTASA + 0x006796CC) = *CModelInfo::ms_modelInfoPtrs;
}

CVehicleModelInfo* CModelInfo::AddVehicleModel(int index)
{
    Log("AddVehicleModel %d", index);
    auto& pInfo = CModelInfo::ms_vehicleModelInfoStore.AddItem();

    ((void(*)(CVehicleModelInfo*))(g_libGTASA + 0x0033559C + 1))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    pInfo.vtable = g_libGTASA + 0x005C6EE0;

    ((void(*)(CVehicleModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CPedModelInfo* CModelInfo::AddPedModel(int index)
{
    Log("CModelInfo_AddPedModel_hook %d", index);

    auto& pInfo = CModelInfo::ms_pedModelInfoStore.AddItem();

    ((void(*)(CPedModelInfo*))(g_libGTASA + 0x0033559C + 1))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    pInfo.vtable = g_libGTASA + 0x005C6E90;

    ((void(*)(CPedModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CAtomicModelInfo* CModelInfo::AddAtomicModel(int index)
{
    Log("AddAtomicModel %d", index);
    auto& pInfo = ms_atomicModelInfoStore.AddItem();

    ((void(*)(CAtomicModelInfo*))(g_libGTASA + 0x0033559C + 1))(&pInfo);

    pInfo.vtable = g_libGTASA + 0x005C6C68;

    ((void(*)(CAtomicModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}


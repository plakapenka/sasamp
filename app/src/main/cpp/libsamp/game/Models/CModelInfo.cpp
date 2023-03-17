//
// Created by plaka on 21.02.2023.
//

#include "CModelInfo.h"
#include "util/patch.h"
#include "CAtomicModelInfo.h"

CBaseModelInfo *CModelInfo::ms_modelInfoPtrs[20000];

CStore<CPedModelInfo, 350> CModelInfo::ms_pedModelInfoStore;
CStore<CAtomicModelInfo, 20000> CModelInfo::ms_atomicModelInfoStore;
CStore<CVehicleModelInfo, 220> CModelInfo::ms_vehicleModelInfoStore;

void CModelInfo::InstallHooks()
{
    CHook::Write(g_libGTASA + 0x00676A34, &CModelInfo::ms_atomicModelInfoStore);
    CHook::Write(g_libGTASA + 0x006773CC, &CModelInfo::ms_pedModelInfoStore);
    CHook::Write(g_libGTASA + 0x00678C8C, &CModelInfo::ms_vehicleModelInfoStore);
    CHook::Write(g_libGTASA + 0x006796CC, &CModelInfo::ms_modelInfoPtrs);

    CHook::Redirect(g_libGTASA, 0x00386114, &CModelInfo::AddPedModel);
    CHook::Redirect(g_libGTASA, 0x003860DC, &CModelInfo::AddVehicleModel);
    CHook::Redirect(g_libGTASA, 0x00385A38, &CModelInfo::AddAtomicModel);

}

CVehicleModelInfo* CModelInfo::AddVehicleModel(int index)
{
    auto& pInfo = CModelInfo::ms_vehicleModelInfoStore.AddItem();

    ((void(*)(CVehicleModelInfo*))(g_libGTASA + 0x00386E98 + 1))(&pInfo); // CVehicleModelInfo::CVehicleModelInfo

    pInfo.vtable = g_libGTASA + 0x006676A8;

    ((void(*)(CVehicleModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CPedModelInfo* CModelInfo::AddPedModel(int index)
{
    auto& pInfo = CModelInfo::ms_pedModelInfoStore.AddItem();

    ((void(*)(CPedModelInfo*))(g_libGTASA + 0x00384FD8 + 1))(&pInfo); // CBaseModelInfo::CBaseModelInfo();

    pInfo.vtable = g_libGTASA + 0x00667658;

    ((void(*)(CPedModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

CAtomicModelInfo* CModelInfo::AddAtomicModel(int index)
{
    auto& pInfo = ms_atomicModelInfoStore.AddItem();

    ((void(*)(CAtomicModelInfo*))(g_libGTASA + 0x00384FD8 + 1))(&pInfo);

    pInfo.vtable = g_libGTASA + 0x00667444;

    ((void(*)(CAtomicModelInfo*))(*(uintptr_t*)(pInfo.vtable + 0x1C)))(&pInfo);

    CModelInfo::SetModelInfo(index, &pInfo);
    return &pInfo;
}

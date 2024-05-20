//
// Created by plaka on 05.04.2023.
//

#include "ColStore.h"
#include "game/constants.h"
#include "util/patch.h"
#include "game/Models/ModelInfo.h"

CColPool* CColStore::ms_pColPool = nullptr;
CQuadTreeNode* CColStore::ms_pQuadTree = nullptr;
bool CColStore::ms_bCollisionNeeded = false;

void CColStore::InjectHooks() {
    CHook::Write(g_libGTASA + 0x00679658, &CColStore::ms_pColPool);
    CHook::Write(g_libGTASA + 0x00679F48, &CColStore::ms_pQuadTree);

    CHook::Write(g_libGTASA + 0x000533EC, &CColStore::ms_bCollisionNeeded);

    CHook::Redirect(g_libGTASA, 0x002E20B0, &CColStore::Initialise);
    CHook::Redirect(g_libGTASA, 0x002E23F4, &CColStore::Shutdown);
    CHook::Redirect(g_libGTASA, 0x002E21A0, &CColStore::AddColSlot);
}

void* ColDef::operator new(unsigned size)
{
    return CColStore::ms_pColPool->New();
}

void ColDef::operator delete(void* data)
{
    CColStore::ms_pColPool->Delete(static_cast<ColDef*>(data));
}

void CColStore::Initialise()
{
    const auto rect = CRect(-3000.0F, -3000.0F, 3000.0F, 3000.0F);
    ms_bCollisionNeeded = false;
    if (!ms_pColPool)
        ms_pColPool = new CColPool(TOTAL_COL_MODEL_IDS, "CollisionFiles");

    AddColSlot("generic");
    ms_pQuadTree = new CQuadTreeNode(rect, 3);
}

void CColStore::Shutdown()
{
    for (auto i = 0; i < ms_pColPool->GetSize(); i++) {
        if (ms_pColPool->GetAt(i)) {
            RemoveColSlot(i);
        }
    }

    delete ms_pColPool;
    ms_pColPool = nullptr;

    delete ms_pQuadTree;
    ms_pQuadTree = nullptr;
}

int32_t CColStore::AddColSlot(const char* name)
{
    auto def = new ColDef();
    def->m_bActive = false;
    def->m_bCollisionIsRequired = false;
    def->m_bProcedural = false;
    def->m_bInterior = false;
    def->m_Area = CRect();
    def->m_nModelIdStart = -1;
    def->m_nModelIdEnd = SHRT_MIN;
    def->m_nRefCount = 0;

    if (!strcmp(name, "procobj") || !strcmp(name, "proc_int") || !strcmp(name, "proc_int2"))
        def->m_bProcedural = true;

    if (!strncmp(name, "int_la", 6)
        || !strncmp(name, "int_sf", 6)
        || !strncmp(name, "int_veg", 7)
        || !strncmp(name, "int_cont", 8)
        || !strncmp(name, "gen_int1", 8)
        || !strncmp(name, "gen_int2", 8)
        || !strncmp(name, "gen_int3", 8)
        || !strncmp(name, "gen_int4", 8)
        || !strncmp(name, "gen_int5", 8)
        || !strncmp(name, "gen_intb", 8)
        || !strncmp(name, "savehous", 8)
        || !strcmp(name, "props")
        || !strcmp(name, "props2")
        || !strncmp(name, "levelmap", 8)
        || !strncmp(name, "stadint", 7))
    {
        def->m_bInterior = true;
    }

    return ms_pColPool->GetIndex(def);
}

void CColStore::AddCollisionNeededAtPosn(const CVector& pos)
{
//    ms_vecCollisionNeeded = pos;
//    ms_bCollisionNeeded = true;
}

void CColStore::AddRef(int32_t colNum)
{
    auto *colDef = ms_pColPool->GetAt(colNum);
    ++colDef->m_nRefCount; //BUG: We don't check whether the GetAt returned nullptr, which it can
}

int32_t CColStore::FindColSlot()
{
    return -1;
}

void CColStore::RemoveCol(int32_t colSlot)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    def->m_bActive = false;
    for (auto i = def->m_nModelIdStart; i <= def->m_nModelIdEnd; ++i) {
        auto* mi = CModelInfo::GetModelInfo(i);
        if (!mi)
            continue;

        auto* cm = mi->GetColModel();
        if (!cm)
            continue;

        if (mi->IsLod() && cm->m_nColSlot == colSlot)
            cm->RemoveCollisionVolumes();
    }
}

void CColStore::RemoveColSlot(int32_t colSlot)
{
    auto* def = ms_pColPool->GetAt(colSlot);
    if (def->m_bActive)
        RemoveCol(colSlot);

    ms_pQuadTree->DeleteItem(def);
    delete def;
}



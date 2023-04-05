//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLSTORE_H
#define LIVERUSSIA_COLSTORE_H


#include "../Core/Rect.h"
#include "../Core/Pool.h"
#include "game/Core/QuadTreeNode.h"

struct ColDef {
    CRect  m_Area;
    uint32_t field_10;
    uint32_t field_14;
    uint32_t field_18;
    uint32_t field_1C;
    uint16_t field_20;
    int16_t  m_nModelIdStart;
    int16_t  m_nModelIdEnd;
    uint16_t m_nRefCount;
    bool   m_bActive;
    bool   m_bCollisionIsRequired;
    bool   m_bProcedural;
    bool   m_bInterior;

    static void* operator new(unsigned size);
    static void  operator delete(void* data);
};

typedef CPool<ColDef> CColPool;

class CColStore {
public:
    static CColPool*       ms_pColPool;
    static CQuadTreeNode*  ms_pQuadTree;

    static CVector      ms_vecCollisionNeeded;
    static bool         ms_bCollisionNeeded;
    static int32_t      ms_nRequiredCollisionArea;

public:
    static void InjectHooks();

    static void Initialise();
    static void Shutdown();
    static int32_t AddColSlot(const char* name);
    static void AddCollisionNeededAtPosn(const CVector& pos);
    static void AddRef(int32_t colNum);
    static int32_t FindColSlot();
    static void BoundingBoxesPostProcess();
    static void EnsureCollisionIsInMemory(const CVector& pos);
    static CRect* GetBoundingBox(int32_t colSlot);
    static void IncludeModelIndex(int32_t colSlot, int32_t modelId);
    static bool HasCollisionLoaded(const CVector& pos, int32_t areaCode);
    static void LoadAllBoundingBoxes();
    static void LoadAllCollision();
    static void LoadCol(int32_t colSlot, const char* filename);
    static bool LoadCol(int32_t colSlot, uint8_t* data, int32_t dataSize);
    static void LoadCollision(CVector pos, bool bIgnorePlayerVeh);
    static void RemoveAllCollision();
    static void RemoveCol(int32_t colSlot);
    static void RemoveColSlot(int32_t colSlot);
    static void RemoveRef(int32_t colNum);
    static void RequestCollision(const CVector& pos, int32_t areaCode);
    static void SetCollisionRequired(const CVector& pos, int32_t areaCode);
};


#endif //LIVERUSSIA_COLSTORE_H

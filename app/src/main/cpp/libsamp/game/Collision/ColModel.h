//
// Created by plaka on 29.03.2023.
//

#ifndef LIVERUSSIA_COLMODEL_H
#define LIVERUSSIA_COLMODEL_H


#include "BoundingBox.h"
#include "CollisionData.h"

class CColModel : public CBoundingBox {
public:
    uint8_t        m_nColSlot;
    union {
        struct {
            uint8_t m_hasCollisionVolumes : 1; // AKA `m_bNotEmpty`
            uint8_t m_useSingleAlloc : 1;
            uint8_t m_bIsActive : 1;
        };
        uint8_t m_nFlags;
    };
    uint8_t undefined_1;
    uint8_t undefined_2;
    CCollisionData* m_pColData;

public:
    static void InjectHooks();

    CColModel();
    ~CColModel();

    static void* operator new(unsigned size);
    static void operator delete(void* data);
    CColModel& operator=(const CColModel& colModel);

public:
    void AllocateData();
    void AllocateData(int32_t numSpheres, int32_t numBoxes, int32_t numLines, int32_t numVertices, int32_t numTriangles, bool bUsesDisks);
    void MakeMultipleAlloc();
    void RemoveCollisionVolumes();
    void CalculateTrianglePlanes();
    void RemoveTrianglePlanes();

private:
    void AllocateData(int32_t size);
};
static_assert(sizeof(CColModel) == 0x30, "Invalid");

#endif //LIVERUSSIA_COLMODEL_H

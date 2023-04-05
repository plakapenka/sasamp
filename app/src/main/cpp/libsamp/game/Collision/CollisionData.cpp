//
// Created by plaka on 29.03.2023.
//

#include "CollisionData.h"
#include "game/MemoryMgr.h"

CCollisionData::CCollisionData() {
    char v1; // r2

    m_nNoOfLines = 0;
    m_nNoOfTriangles = 0;
    m_nNoOfSpheres = 0;
    m_pShadTriangleArray = nullptr;
    v1 = *((uintptr_t *) this + 7);

    m_pTriangleArray = nullptr;
    m_nNoOfShadTriangles = 0LL;
    m_pSphereArray = nullptr;
    m_pLineArray = nullptr;
    m_pTriCompressedShadVectorArray = nullptr;

    *((uintptr_t *) this + 7) = v1 & 0xF8;
}

CLink<CCollisionData*>* CCollisionData::GetLinkPtr() {
    // Original calculation method:
    // const auto dwLinkAddress = (reinterpret_cast<uint32>(&m_pTrianglePlanes[m_nNumTriangles]) + 3) & 0xFFFFFFFC; // 4 bytes aligned address

    auto* linkPtr = static_cast<void*>(&m_pTrianglePlanes[m_nNoOfTriangles]);
    auto space = sizeof(CColTrianglePlane);
    auto* alignedAddress = std::align(4, sizeof(CLink<CCollisionData*>*), linkPtr, space); // 4 bytes aligned address
    return *static_cast<CLink<CCollisionData*>**>(alignedAddress);
}
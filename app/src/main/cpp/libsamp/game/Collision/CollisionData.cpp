//
// Created by plaka on 29.03.2023.
//

#include "CollisionData.h"

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
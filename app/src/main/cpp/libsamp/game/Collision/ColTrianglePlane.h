//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLTRIANGLEPLANE_H
#define LIVERUSSIA_COLTRIANGLEPLANE_H

#include "../Core/Vector.h"

struct __attribute__((aligned(4))) CColTrianglePlane
{
    CVector m_compressedNormal;
    float m_compressedNormalOffset;
    uint8_t m_nPrincipalAxis;
};
static_assert(sizeof(CColTrianglePlane) == 0x14, "Invalid size CCollisionData");

#endif //LIVERUSSIA_COLTRIANGLEPLANE_H

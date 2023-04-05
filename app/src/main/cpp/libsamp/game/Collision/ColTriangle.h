//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLTRIANGLE_H
#define LIVERUSSIA_COLTRIANGLE_H

struct __attribute__((aligned(4))) CColTriangle
{
    int32_t m_nIndex1;
    int32_t m_nIndex2;
    int32_t m_nIndex3;
    uint8_t m_nSurfaceType;
    uint8_t m_nLighting;
};
static_assert(sizeof(CColTriangle) == 0x10, "Invalid");

#endif //LIVERUSSIA_COLTRIANGLE_H

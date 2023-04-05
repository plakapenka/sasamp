//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLSURFACE_H
#define LIVERUSSIA_COLSURFACE_H

#include "../Enums/SurfaceType.h"
#include "ColData.h"

struct CColSurface {
    eSurfaceType m_nMaterial{};
    uint8_t        m_nPiece{}; // ePedPieceTypes, eCarPiece, etc
    tColLighting m_nLighting{};
    uint8_t        m_nLight{};
};
static_assert(sizeof(CColSurface) == 0x4, "Invalid");

#endif //LIVERUSSIA_COLSURFACE_H

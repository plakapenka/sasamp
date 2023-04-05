//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLBOX_H
#define LIVERUSSIA_COLBOX_H


#include "ColData.h"
#include "../Core/CVector.h"
#include "Box.h"
#include "ColSurface.h"

class CColBox : public CBox {
public:
    CColSurface m_Surface;

public:
    constexpr CColBox() = default;
    constexpr CColBox(const CVector& min, const CVector& max) : CBox(min, max) {}
    constexpr CColBox(const CBox& box) : CBox(box) {}
    constexpr CColBox(const CBox& box, eSurfaceType material, uint8_t pieceType, tColLighting lightning) : CBox(box) {
        m_Surface.m_nMaterial = material;
        m_Surface.m_nPiece = pieceType;
        m_Surface.m_nLighting = lightning;
    }

    void Set(const CVector& sup, const CVector& inf, eSurfaceType material, uint8_t pieceType, tColLighting lighting);
    CColBox& operator=(const CColBox& right);

    auto GetSurfaceType() const { return m_Surface.m_nMaterial; }
};
static_assert(sizeof(CColBox) == 0x1c, "Invalid");

#endif //LIVERUSSIA_COLBOX_H

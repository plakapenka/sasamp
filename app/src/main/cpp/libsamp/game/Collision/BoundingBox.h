//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_BOUNDINGBOX_H
#define LIVERUSSIA_BOUNDINGBOX_H


#include "Box.h"
#include "Sphere.h"

class CBoundingBox : public CBox {
public:
    CSphere      m_boundSphere;

    constexpr CBoundingBox() : CBox(CVector{1.0f}, CVector{-1.0f}) {}
    constexpr CBoundingBox(CVector min, CVector max) : CBox(min, max) {}
    constexpr explicit CBoundingBox(const CBox& box) : CBox(box) {}

    [[nodiscard]] bool IsPointWithin(const CVector& point) const;
    inline void SetMinMax(CVector min, CVector max);
};
static_assert(sizeof(CBoundingBox) == 0x28, "Invalid");

#endif //LIVERUSSIA_BOUNDINGBOX_H

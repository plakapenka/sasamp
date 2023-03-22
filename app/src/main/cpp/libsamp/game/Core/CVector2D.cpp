//
// Created by plaka on 23.03.2023.
//

#include "CVector2D.h"
#include "CVector.h"
#include "game/CGeneral.h"

CVector2D::CVector2D(const CVector& v3) :
        CVector2D{v3.x, v3.y}
{
}

void CVector2D::Normalise() {
    auto len = Magnitude();
    if (len > 0.0f) {
        auto recip = 1.0F / len;
        x *= recip;
        y *= recip;
    } else {
        x = 1.0f;
    }
}

uint32_t CVector2D::NodeHeading() const {
    return CGeneral::GetNodeHeadingFromVector(x, y);
}

CVector2D CVector2D::RotatedBy(float rad) const {
    const auto s = std::sin(rad), c = std::cos(rad);
    return {
            x * c - y * s,
            x * s - y * c,
    };
}

CVector2D CVector2D::GetPerpRight() const {
    return { y, -x }; // `RotatedBy(-PI / 2)` done manually, rotate by +PI/2 would be `{-y, x}`
}

CVector2D CVector2D::GetPerpLeft() const {
    return { -y, x };
}
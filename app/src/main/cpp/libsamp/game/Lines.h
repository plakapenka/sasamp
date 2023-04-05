//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_LINES_H
#define LIVERUSSIA_LINES_H


#include "Core/Vector.h"
#include "rgba.h"
#include "game/RW/rwcore.h"

class CLines {
public:
    static void RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32_t startColor, uint32_t endColor);
    static void RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32_t startColor, uint32_t endColor);
    static void ImmediateLine2D(int32_t startX, int32_t startY, int32_t endX, int32_t endY, uint8_t startR, uint8_t startG, uint8_t startB, uint8_t startA, uint8_t endR, uint8_t endG, uint8_t endB, uint8_t endA);

public:
    // NOTSA wrappers
    static void RenderLineNoClipping(const CVector& start, const CVector& end, uint32_t startColor, uint32_t endColor) {
        RenderLineNoClipping(start.x, start.y, start.z, end.x, end.y, end.z, startColor, endColor);
    }

    static void RenderLineWithClipping(const CVector& start, const CVector& end, uint32_t startColor, uint32_t endColor) {
        RenderLineWithClipping(start.x, start.y, start.z, end.x, end.y, end.z, startColor, endColor);
    }

    static void ImmediateLine2D(const CVector2D& start, const CVector2D& end, const CRGBA& startColor, const CRGBA& endColor) {
        ImmediateLine2D(
                static_cast<int32_t>(start.x), static_cast<int32_t>(start.y),
                static_cast<int32_t>(end.x), static_cast<int32_t>(end.y),
                startColor.R, startColor.G, startColor.B, startColor.A,
                endColor.R, endColor.G, endColor.B, endColor.A
        );
    }
};


#endif //LIVERUSSIA_LINES_H

//
// Created by plaka on 23.03.2023.
//

#include "CGeneral.h"
#include "common.h"
#include "util/util.h"

uint32_t CGeneral::GetNodeHeadingFromVector(float x, float y) {
    float angle = GetRadianAngleBetweenPoints(x, y, 0.0f, 0.0f);
    if (angle < 0.0f)
        angle += TWO_PI;

    angle = TWO_PI - angle + CUtil::DegToRad(22.5f);

    if (angle >= TWO_PI)
        angle -= TWO_PI;

    return (uint32_t)floor(angle / CUtil::DegToRad(45.0f));
}

float CGeneral::GetRadianAngleBetweenPoints(CVector2D a, CVector2D b) {
    return GetRadianAngleBetweenPoints(a.x, a.y, b.x, b.y);
}

float CGeneral::GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    float x = x2 - x1;
    float y = y2 - y1;

    if (y == 0.0f)
        y = 0.0001f;

    if (x > 0.0f) {
        if (y > 0.0f)
            return PI - atan2(x / y, 1.0f);
        else
            return -atan2(x / y, 1.0f);
    } else {
        if (y > 0.0f)
            return -(PI + atan2(x / y, 1.0f));
        else
            return -atan2(x / y, 1.0f);
    }
}
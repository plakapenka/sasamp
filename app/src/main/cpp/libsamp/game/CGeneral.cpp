//
// Created by plaka on 23.03.2023.
//

#include <cassert>
#include "CGeneral.h"
#include "common.h"
#include "util/util.h"

float CGeneral::LimitAngle(float angle) {
    while (angle >= 180.0f) {
        angle -= 360.0f;
    }

    while (angle < -180.0f) {
        angle += 360.0f;
    }

    return angle;
}

// 0x53CB50
float CGeneral::LimitRadianAngle(float angle) {
    float result = std::clamp(angle, -25.0f, 25.0f);

    while (result >= PI) {
        result -= 2 * PI;
    }

    while (result < -PI) {
        result += 2 * PI;
    }

    return result;
}

// 0x53CBE0
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

// 0x53CC70
float CGeneral::GetATanOfXY(float x, float y) {
    if (x == 0.0f && y == 0.0f)
        return 0.0f;

    // Wikipedia explains this function in great detail: https://en.wikipedia.org/wiki/Atan2

    float xabs = abs(x);
    float yabs = abs(y);

    if (xabs < yabs) {
        if (y > 0.0f) {
            if (x > 0.0f)
                return 0.5f * PI - atan2(x / y, 1.0f);
            else
                return 0.5f * PI + atan2(-x / y, 1.0f);
        } else {
            if (x > 0.0f)
                return 1.5f * PI + atan2(x / -y, 1.0f);
            else
                return 1.5f * PI - atan2(-x / -y, 1.0f);
        }
    } else {
        if (y > 0.0f) {
            if (x > 0.0f)
                return atan2(y / x, 1.0f);
            else
                return PI - atan2(y / -x, 1.0f);
        } else {
            if (x > 0.0f)
                return 2.0f * PI - atan2(-y / x, 1.0f);
            else
                return PI + atan2(-y / -x, 1.0f);
        }
    }
}

// 0x53CDC0
uint32_t CGeneral::GetNodeHeadingFromVector(float x, float y) {
    float angle = GetRadianAngleBetweenPoints(x, y, 0.0f, 0.0f);
    if (angle < 0.0f)
        angle += TWO_PI;

    angle = TWO_PI - angle + CUtil::DegToRad(22.5f);

    if (angle >= TWO_PI)
        angle -= TWO_PI;

    return (uint32_t)floor(angle / CUtil::DegToRad(45.0f));
}

// 0x53CE30
bool CGeneral::SolveQuadratic(float a, float b, float c, float& x1, float& x2) {
    float discriminant = b * b - 4.f * a * c;
    if (discriminant < 0.0f)
        return false;

    float discriminantSqrt = sqrt(discriminant);
    x2 = (-b + discriminantSqrt) / (2.0f * a);
    x1 = (-b - discriminantSqrt) / (2.0f * a);
    return true;
}

// 0x53CEA0
float CGeneral::GetAngleBetweenPoints(float x1, float y1, float x2, float y2) {
    return CUtil::RadiansToDegrees(GetRadianAngleBetweenPoints(x1, y1, x2, y2));
}

uint16_t CGeneral::GetRandomNumber() { // Why does this return `uint16`? Should be `int32` (same as retval of `rand()`)

    return rand() % 32767;
}

float CGeneral::GetRadianAngleBetweenPoints(CVector2D a, CVector2D b) {
    return GetRadianAngleBetweenPoints(a.x, a.y, b.x, b.y);
}

bool CGeneral::RandomBool(float chanceOfTrue) {
    assert(chanceOfTrue <= 100.f);
    return CGeneral::GetRandomNumberInRange(0.f, 100.f) <= chanceOfTrue;
}

/*!
* @return true/false with 50/50 change
* @addr notsa
*/
bool CGeneral::DoCoinFlip() {
    return CGeneral::GetRandomNumber() >= RAND_MAX / 2;
}
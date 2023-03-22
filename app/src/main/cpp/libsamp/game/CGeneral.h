//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_CGENERAL_H
#define LIVERUSSIA_CGENERAL_H


#include <cstdint>
#include "game/Core/CVector2D.h"

namespace CGeneral {
    /*!
     * @addr 0x53CDC0
     * @returns Get the octant the vector's heading lies in
     */
    uint32_t GetNodeHeadingFromVector(float x, float y);

    /*!
     * @addr 0x53CBE0
     * @returns Angle [in radians] between 2 points (Prefer using `(p1 - p2).Heading()` instead!)
     */
    float GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2);

    /*!
     * @returns Angle [in degrees] between 2 points (Prefer using `(p1 - p2).Heading()` instead [NOTE: The latter returns radians, not degrees])
     */
    float GetRadianAngleBetweenPoints(CVector2D a, CVector2D b);

};


#endif //LIVERUSSIA_CGENERAL_H

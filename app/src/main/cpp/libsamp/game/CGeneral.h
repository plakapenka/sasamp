//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_CGENERAL_H
#define LIVERUSSIA_CGENERAL_H


#include <cstdlib>
#include <functional>
#include "game/Core/CVector2D.h"

//! If you see this value, USE THE FLOAT VERSION of `GetRandomNumberInRange`
constexpr float RAND_MAX_FLOAT_RECIPROCAL = 1.0f / static_cast<float>(RAND_MAX);   // 1.0 / 32767.0 == 1.0 / RAND_MAX       = 0.0000305185094

namespace CGeneral { // More like `Math` (Or `Meth`, given how bad the code is, these guys must've been high!)

    /*!
     * @addr 0x53CB00
     * @brief Normalize the `angle` to be between [-180, 180] deg
     */
    float LimitAngle(float angle);

    /*!
     * @addr 0x53CB50
     * @returns The `angle` normalized to be between [-pi, pi]
     */
    float LimitRadianAngle(float angle);

    /*!
     * @addr 0x53CC70
     * @returns atan2(??, ??) - TODO
     */
    float GetATanOfXY(float x, float y);

    /*!
     * @notsa
     * @copydoc GetATanOfXY
     */
    inline float GetATanOf(CVector2D point) { return GetATanOfXY(point.x, point.y); } // This just returns the heading most likely, so `point.Heading()` should work too [?]

    /*!
     * @addr 0x53CDC0
     * @returns Get the octant the vector's heading lies in
     */
    uint32_t GetNodeHeadingFromVector(float x, float y);

    /*!
     * @addr 0x53CE30
     * @brief Try solving a quadratic equation
     * @returns If the equation had a solution
     */
    bool SolveQuadratic(float a, float b, float c, float& x1, float& x2);

    /*!
     * @addr 0x53CBE0
     * @returns Angle [in radians] between 2 points (Prefer using `(p1 - p2).Heading()` instead!)
     */
    float GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2);

    /*!
     * @addr 0x53CEA0
     * @returns Angle [in degrees] between 2 points (Prefer using `(p1 - p2).Heading()` instead [NOTE: The latter returns radians, not degrees])
     */
    float GetAngleBetweenPoints(float x1, float y1, float x2, float y2);

    /*!
     * @returns Angle [in degrees] between 2 points (Prefer using `(p1 - p2).Heading()` instead [NOTE: The latter returns radians, not degrees])
     */
    float GetRadianAngleBetweenPoints(CVector2D a, CVector2D b);

    /*!
     * @returns A random number in range [0, RAND_MAX] (Same as `rand()`)
     */
    uint16_t GetRandomNumber();

    /*!
     * @returns True `chanceOfTrue` % of the time, false othertimes. Valid value are [0, 100)
     */
    bool RandomBool(float chanceOfTrue);

    /*!
     * @returns True 50% of the time
     */
    bool DoCoinFlip();

    /**
     * @addr 0x41BD90
     * @param min Minimum value
     * @param max Maximum value. Must be greater than min.
     * @return A pseudo-random number between min and max, inclusive [min, max].
     */

    inline float GetRandomNumberInRange(float min, float max) {

        return std::lerp(min, max, static_cast<float>(GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL);
    }



    /*
    * @notsa
    * @brief Return a random node heading, or direction as commonly referred to. See `GetNodeHeadingFromVector`
    */
    inline auto RandomNodeHeading() {
        return (uint8_t)CGeneral::GetRandomNumberInRange(0, 8);
    }
};



#endif //LIVERUSSIA_CGENERAL_H

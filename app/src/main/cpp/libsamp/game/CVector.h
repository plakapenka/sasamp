//
// Created by plaka on 30.01.2023.
//

#ifndef LIVERUSSIA_CVECTOR_H
#define LIVERUSSIA_CVECTOR_H


#include "game/RW/rwlpcore.h"

class CVector : public RwV3d {
public:
    CVector();
    CVector(float X, float Y, float Z);
    CVector(RwV3d rwVec) { x = rwVec.x; y = rwVec.y; z = rwVec.z; }
public:
    CVector(float value);

// Returns length of vector
    float Magnitude();

    // Returns length of 2d vector
    float Magnitude2D();

    // Normalises a vector
    void Normalise();

    // Normalises a vector and returns length
    float NormaliseAndMag();

    // Performs cross calculation
    void Cross(const CVector& left, const CVector &right);

    // Adds left + right and stores result
    void Sum(const CVector& left, const CVector &right);

    // Subtracts left - right and stores result
    void Difference(const CVector& left, const CVector &right);

    void operator=(const RwV3d& right)
    {
        x = right.x; y = right.y; z = right.z;
    }

    void operator=(const CVector& right);
    void operator+=(const CVector& right);
    void operator-=(const CVector& right);
    void operator*=(const CVector& right);
    void operator *= (float multiplier);
    void operator /= (float divisor);
};


#endif //LIVERUSSIA_CVECTOR_H

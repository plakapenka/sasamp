//
// Created by plaka on 30.01.2023.
//

#include "CVector.h"
#include "CVector2D.h"
#include "../CGeneral.h"
#include "CMatrix.h"
#include <cmath>

#include <numeric>
/*!
* @brief From a 2D vector and Z position
*/
CVector::CVector(const CVector2D& v2d, float Z) :
        RwV3d{ v2d.x, v2d.y, Z }
{
}

/*!
* @returns A vector with each of its components set to a number in the given range [min, max)
*/
CVector CVector::Random(CVector min, CVector max) {
    const auto Get = [=](float fmin, float fmax) { return CGeneral::GetRandomNumberInRange(fmin, fmax); };
    return { Get(min.x, max.x), Get(min.y, max.y), Get(min.z, max.z) };
}

CVector CVector::Random(float min, float max) {
    const auto Get = [=] { return CGeneral::GetRandomNumberInRange(min, max); };
    return { Get(), Get(), Get() };
}

// Returns length of vector
float CVector::Magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}

float CVector::Magnitude2D() const
{
    return sqrt(x * x + y * y);
}

// Normalises a vector
void CVector::Normalise()
{
    NormaliseAndMag();
}

// Normalises a vector and returns length
float CVector::NormaliseAndMag()
{
    const auto fDot = x * x + y * y + z * z;
    if (fDot <= 0.0F)
    {
        x = 1.0F;
        return 1.0F;
    }

    const auto fRecip = 1.0F / sqrt(fDot);
    x *= fRecip;
    y *= fRecip;
    z *= fRecip;

    return 1.0F / fRecip;
}

auto CVector::Normalized() const -> CVector {
    CVector cpy = *this;
    cpy.Normalise();
    return cpy;
}

auto CVector::Dot(const CVector& o) const -> float{
    return DotProduct(*this, o);
}

// notsa
CVector CVector::Cross(const CVector& o) const {
    return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
    };
}

// 0x70F890
void CVector::Cross_OG(const CVector& a, const CVector& b) {
    *this = a.Cross(b);
}

// Adds left + right and stores result
void CVector::Sum(const CVector& left, const CVector &right)
{
    x = left.x + right.x;
    y = left.y + right.y;
    z = left.z + right.z;
}

// Subtracts left - right and stores result
void CVector::Difference(const CVector& left, const CVector &right)
{
    x = left.x - right.x;
    y = left.y - right.y;
    z = left.z - right.z;
}

// Adds value from the second vector.
void CVector::operator+=(const CVector& right)
{
    x += right.x;
    y += right.y;
    z += right.z;
}

void CVector::operator-=(const CVector& right)
{
    x -= right.x;
    y -= right.y;
    z -= right.z;
}

void CVector::operator*=(const CVector& right)
{
    x *= right.x;
    y *= right.y;
    z *= right.z;
}

// Multiplies vector by a floating point value
void CVector::operator *= (float multiplier)
{
    x *= multiplier;
    y *= multiplier;
    z *= multiplier;
}

// Divides vector by a floating point value
void CVector::operator /= (float divisor)
{
    x /= divisor;
    y /= divisor;
    z /= divisor;
}

void CVector::FromMultiply(const CMatrix& matrix, const CVector& vector)
{
    x = matrix.pos.x + matrix.right.x * vector.x + matrix.up.x * vector.y + matrix.at.x * vector.z;
    y = matrix.pos.y + matrix.right.y * vector.x + matrix.up.y * vector.y + matrix.at.y * vector.z;
    z = matrix.pos.z + matrix.right.z * vector.x + matrix.up.z * vector.y + matrix.at.z * vector.z;
}

void CVector::FromMultiply3x3(const CMatrix& matrix, const CVector& vector)
{
    x = matrix.right.x * vector.x + matrix.up.x * vector.y + matrix.at.x * vector.z;
    y = matrix.right.y * vector.x + matrix.up.y * vector.y + matrix.at.y * vector.z;
    z = matrix.right.z * vector.x + matrix.up.z * vector.y + matrix.at.z * vector.z;
}

CVector CVector::Average(const CVector* begin, const CVector* end) {
    return std::accumulate(begin, end, CVector{}) / (float)std::distance(begin, end);
}

float CVector::Heading(bool limitAngle) const {
    const auto heading = std::atan2(-x, y);
    if (limitAngle) {
        return CGeneral::LimitRadianAngle(heading);
    }
    return heading;
}


CVector* CrossProduct(CVector* out, CVector* a, CVector* b)
{
    *out = a->Cross(b);
    return out;
}

CVector CrossProduct(const CVector& a, const CVector& b)
{
    return a.Cross(b);
}

float DotProduct(CVector* v1, CVector* v2)
{
    return v1->z * v2->z + v1->y * v2->y + v1->x * v2->x;
}

float DotProduct(const CVector& v1, const CVector& v2)
{
    return v1.z * v2.z + v1.y * v2.y + v1.x * v2.x;
}

float DotProduct2D(const CVector& v1, const CVector& v2)
{
    return v1.y * v2.y + v1.x * v2.x;
}

// NOTE: This function doesn't add m.GetPosition() like
//       MultiplyMatrixWithVector @ 0x59C890 does.
CVector Multiply3x3(const CMatrix& constm, const CVector& v) {
    auto& m = const_cast<CMatrix&>(constm);
    return {
            m.GetRight().x * v.x + m.GetForward().x * v.y + m.GetUp().x * v.z,
            m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetUp().y * v.z,
            m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetUp().z * v.z,
    };
}

// vector by matrix mult, resulting in a vector where each component is the dot product of the in vector and a matrix direction
CVector Multiply3x3(const CVector& v, const CMatrix& constm) {
    auto& m = const_cast<CMatrix&>(constm);
    return {
            DotProduct(m.GetRight(), v),
            DotProduct(m.GetForward(), v),
            DotProduct(m.GetUp(), v)
    };
}

CVector MultiplyMatrixWithVector(const CMatrix& mat, const CVector& vec) {
    return const_cast<CMatrix&>(mat).GetPosition() + Multiply3x3(const_cast<CMatrix&>(mat), vec);
}

CVector MultiplyMatrixWithVector(CMatrix& m, const CVector& v) {
    return m.GetPosition() + Multiply3x3(m, v);
}

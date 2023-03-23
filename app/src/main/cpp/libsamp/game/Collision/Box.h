//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_BOX_H
#define LIVERUSSIA_BOX_H


#include "../Core/CVector.h"
#include "../rgba.h"

class CBox {
public:
    CVector m_vecMin{}, m_vecMax{};

    constexpr CBox() = default;
    constexpr CBox(CVector min, CVector max) : m_vecMin(min), m_vecMax(max) {}

    void Set(const CVector& vecMin, const CVector& vecMax);

    //! Updates box corners, like (if left>right then swap(left, right))
    void Recalc();

    CVector GetSize()   const { return m_vecMax - m_vecMin; }
    float   GetWidth()  const { return m_vecMax.x - m_vecMin.x; }
    float   GetLength() const { return m_vecMax.y - m_vecMin.y; }
    float   GetHeight() const { return m_vecMax.z - m_vecMin.z; }
    CVector GetCenter() const { return (m_vecMax + m_vecMin) / 2.f; }

};


#endif //LIVERUSSIA_BOX_H

//
// Created by plaka on 23.03.2023.
//

#include "Box.h"

void CBox::Set(const CVector& vecMin, const CVector& vecMax)
{
    m_vecMin = vecMin;
    m_vecMax = vecMax;
}

void CBox::Recalc()
{
    if (m_vecMin.x > m_vecMax.x)
        std::swap(m_vecMin.x, m_vecMax.x);

    if (m_vecMin.y > m_vecMax.y)
        std::swap(m_vecMin.y, m_vecMax.y);

    if (m_vecMin.z > m_vecMax.z)
        std::swap(m_vecMin.z, m_vecMax.z);
}

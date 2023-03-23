//
// Created by plaka on 23.03.2023.
//

#include "BoundingBox.h"

bool CBoundingBox::IsPointWithin(const CVector& point) const {
    return point.x >= m_vecMin.x
           && point.y >= m_vecMin.y
           && point.z >= m_vecMin.z
           && point.x <= m_vecMax.x
           && point.y <= m_vecMax.y
           && point.z <= m_vecMax.z;
}

// 0x470100
void CBoundingBox::SetMinMax(CVector min, CVector max) {
    m_vecMin = min;
    m_vecMax = max;
}

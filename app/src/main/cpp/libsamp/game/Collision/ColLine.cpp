//
// Created by plaka on 23.03.2023.
//

#include "ColLine.h"

CColLine::CColLine(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// 0x40EF10
void CColLine::Set(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// NOTSA
//float CColLine::DistTo(CVector pt) const {
//    return CCollision::DistToLine(m_vecStart, m_vecEnd, pt);
//}
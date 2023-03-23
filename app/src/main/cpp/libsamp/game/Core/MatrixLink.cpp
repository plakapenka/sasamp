//
// Created by plaka on 23.03.2023.
//

#include "MatrixLink.h"

void CMatrixLink::Insert(CMatrixLink* pWhere)
{
    pWhere->m_pNext = m_pNext;
    m_pNext->m_pPrev = pWhere;

    pWhere->m_pPrev = this;
    m_pNext = pWhere;
}

void CMatrixLink::Remove()
{
    m_pNext->m_pPrev = m_pPrev;
    m_pPrev->m_pNext = m_pNext;
}
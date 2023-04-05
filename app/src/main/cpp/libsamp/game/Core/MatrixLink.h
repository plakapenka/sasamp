//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_MATRIXLINK_H
#define LIVERUSSIA_MATRIXLINK_H


#include "Matrix.h"

class CPlaceable;

class CMatrixLink : public CMatrix {
public:
    CMatrixLink() : CMatrix() {}
 //   CMatrixLink(float fScale) : CMatrix() { SetScale(fScale); }

public:
    CPlaceable*  m_pOwner;
    CMatrixLink* m_pPrev;
    CMatrixLink* m_pNext;

public:

    inline void Insert(CMatrixLink* pWhere);
    inline void Remove();
};


#endif //LIVERUSSIA_MATRIXLINK_H

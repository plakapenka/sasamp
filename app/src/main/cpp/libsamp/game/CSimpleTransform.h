//
// Created by plaka on 20.03.2023.
//

#ifndef LIVERUSSIA_CSIMPLETRANSFORM_H
#define LIVERUSSIA_CSIMPLETRANSFORM_H


#include "game/Core/CVector.h"

class CSimpleTransform {
public:
    CSimpleTransform() : m_vPosn(), m_fHeading(0.0F) {}

public:
    CVector m_vPosn;
    float m_fHeading;

    void UpdateRwMatrix(RwMatrix* out);
    void Invert(const CSimpleTransform& base);
    void UpdateMatrix(class CMatrix* out);
};

#endif //LIVERUSSIA_CSIMPLETRANSFORM_H

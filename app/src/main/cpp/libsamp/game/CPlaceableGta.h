//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CPLACEABLEGTA_H
#define LIVERUSSIA_CPLACEABLEGTA_H

#include "game/Core/Quaternion.h"
#include "CSimpleTransform.h"
#include "game/Core/MatrixLink.h"

#pragma pack(1)
struct CPlaceableGta
{
    uint32_t vtable;
    CSimpleTransform m_transform;
    CMatrixLink *mat;
};

#endif //LIVERUSSIA_CPLACEABLEGTA_H

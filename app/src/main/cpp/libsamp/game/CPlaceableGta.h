//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CPLACEABLEGTA_H
#define LIVERUSSIA_CPLACEABLEGTA_H

#include "quaternion.h"

#pragma pack(1)
struct CPlaceableGta
{
    uint32_t vtable;
    uint8_t skip0[16];
    RwMatrix *mat;
};

#endif //LIVERUSSIA_CPLACEABLEGTA_H

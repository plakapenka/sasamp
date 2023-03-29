//
// Created by plaka on 29.03.2023.
//

#ifndef LIVERUSSIA_COLMODEL_H
#define LIVERUSSIA_COLMODEL_H


#include "BoundingBox.h"
#include "CollisionData.h"

class CColModel : public CBoundingBox {
    uint8_t        m_nColSlot;
    union {
        struct {
            uint8_t m_bHasCollisionVolumes : 1; // AKA `m_bNotEmpty`
            uint8_t m_bIsSingleColDataAlloc : 1;
            uint8_t m_bIsActive : 1;
        };
        uint8_t m_nFlags;
    };
    uint8_t undefined_1;
    uint8_t undefined_2;
    CCollisionData* m_pColData;
};
static_assert(sizeof(CColModel) == 0x30, "Invalid");

#endif //LIVERUSSIA_COLMODEL_H

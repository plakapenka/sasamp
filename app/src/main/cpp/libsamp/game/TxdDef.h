//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_TXDDEF_H
#define LIVERUSSIA_TXDDEF_H

#include <cstdint>
#include "RW/rwcore.h"

class TxdDef {
public:
    RwTexDictionary*    m_pRwDictionary;
    int16_t             m_wRefsCount;
    int16_t             m_wParentIndex;
    uint32_t            m_hash;
};

#endif //LIVERUSSIA_TXDDEF_H

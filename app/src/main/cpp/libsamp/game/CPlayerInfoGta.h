//
// Created by plaka on 20.02.2023.
//

#include <cstdint>
#include "CPedGta.h"

#ifndef LIVERUSSIA_CPLAYERINFOGTA_H
#define LIVERUSSIA_CPLAYERINFOGTA_H

struct CPlayerInfoGta {
    CPedGta *m_pPed;
    uint8_t skip[400];
};

#endif //LIVERUSSIA_CPLAYERINFOGTA_H

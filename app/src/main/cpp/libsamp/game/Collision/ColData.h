//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_COLDATA_H
#define LIVERUSSIA_COLDATA_H

#include "../Enums/eSurfaceType.h"

struct tColLighting {
    union {
        struct {
            uint8_t day : 4;
            uint8_t night : 4;
        };
        uint8_t value;
    };

    tColLighting() = default;
    constexpr explicit tColLighting(const uint8_t ucLighting) {
        day = ucLighting & 0xF;
        night = (ucLighting >> 4) & 0xF;
    }

    [[nodiscard]] float GetCurrentLighting(float fScale = 0.5F) const;
};

struct ColData {
    eSurfaceType    m_nSurfaceTypeA;
    uint8_t         m_nPieceTypeA;
    tColLighting    m_nLightingA;
};

// sizeof=0x3

#endif //LIVERUSSIA_COLDATA_H

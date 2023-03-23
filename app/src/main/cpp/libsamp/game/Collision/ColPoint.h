//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_COLPOINT_H
#define LIVERUSSIA_COLPOINT_H


#include "../Core/CVector.h"
#include "ColData.h"

class CColPoint {
public:
    /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CColPointSA.h */
    CVector      m_vecPoint;        // 0x00
    float        field_C;           // 0x0C
    CVector      m_vecNormal;       // 0x10
    float        field_1C;          // 0x1C

    // col shape 1 info
    ColData m_dataA;

    //
    ColData m_dataB;

    char _align0x26[2];             // 0x26

    float m_fDepth;                 // 0x28

public:
    CColPoint& operator=(CColPoint const&) = default;
};


#endif //LIVERUSSIA_COLPOINT_H

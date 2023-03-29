//
// Created by plaka on 29.03.2023.
//

#ifndef LIVERUSSIA_COLSPHERE_H
#define LIVERUSSIA_COLSPHERE_H


#include "Sphere.h"
#include "ColData.h"

class CColSphere : public CSphere{
    ColData m_data;
    uint8_t undefined;
};
static_assert(sizeof(CColSphere) == 0x14, "Invalid size CColSphere");


#endif //LIVERUSSIA_COLSPHERE_H

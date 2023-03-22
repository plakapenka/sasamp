//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_RPWORLD_H
#define LIVERUSSIA_RPWORLD_H

#define RpAtomicGetGeometryMacro(_atomic)                               \
    ((_atomic)->geometry)

#define RpAtomicGetGeometry(_atomic) \
    RpAtomicGetGeometryMacro(_atomic)

#define RpMaterialGetColorMacro(_material)                          \
    (&((_material)->color))

#define RpMaterialGetColor(_material)                               \
    RpMaterialGetColorMacro(_material)

#include "../common.h"
#include "rtanim.h"

#endif //LIVERUSSIA_RPWORLD_H

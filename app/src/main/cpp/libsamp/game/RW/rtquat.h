//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_RTQUAT_H
#define LIVERUSSIA_RTQUAT_H


#include "rwlpcore.h"

typedef struct RtQuat RtQuat;

struct RtQuat
{
    RwV3d               imag;   /**< The imaginary part(s) */
    RwReal              real;   /**< The real part */
};

typedef RtQuat RpQuat;

#endif //LIVERUSSIA_RTQUAT_H

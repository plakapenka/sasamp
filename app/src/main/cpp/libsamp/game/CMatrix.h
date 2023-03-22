//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CMATRIX_H
#define LIVERUSSIA_CMATRIX_H

#include "game/Core/CVector.h"

struct RwMatrixTag
{
    CVector right;		// 0-12 	; r11 r12 r13
    uint32_t  flags;	// 12-16
    CVector up;			// 16-28	; r21 r22 r23
    float  pad_u;		// 28-32
    CVector at;			// 32-44	; r31 r32 r33
    float  pad_a;		// 44-48
    CVector pos;			// 48-60
    float  pad_p;		// 60-64
};
typedef RwMatrixTag RwMatrix;

#endif //LIVERUSSIA_CMATRIX_H

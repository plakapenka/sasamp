//
// Created by plaka on 07.03.2023.
//

#ifndef LIVERUSSIA_CCLUMPMODELINFO_H
#define LIVERUSSIA_CCLUMPMODELINFO_H


#include "CBaseModelInfo.h"

struct CClumpModelInfo : public CBaseModelInfo {
    union {
        char *m_animFileName;
        unsigned int m_dwAnimFileIndex;
    };
};
// 0x3C


#endif //LIVERUSSIA_CCLUMPMODELINFO_H

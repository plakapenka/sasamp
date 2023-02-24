//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_CCLUMPMODELINFO_H
#define LIVERUSSIA_CCLUMPMODELINFO_H

#include "CBaseModelInfo.h"

struct tCompSearchStructById {
    int32_t m_nId;
    RwFrame* m_pFrame;

    inline tCompSearchStructById(int32_t id, RwFrame* frame) : m_nId(id), m_pFrame(frame) {}
};

struct CClumpModelInfo : public CBaseModelInfo {
    union {
        char *m_animFileName;
        unsigned int m_dwAnimFileIndex;
    };
};
// 3C

#endif //LIVERUSSIA_CCLUMPMODELINFO_H

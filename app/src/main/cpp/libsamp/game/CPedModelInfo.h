//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_CPEDMODELINFO_H
#define LIVERUSSIA_CPEDMODELINFO_H


#include "CClumpModelInfo.h"

struct CPedModelInfo : public CClumpModelInfo{
public:
    int m_nAnimType;
    unsigned int m_nPedType;
    unsigned int m_nStatType;
    unsigned short m_nCarsCanDriveMask;
    unsigned short m_nPedFlags;
    uintptr_t *m_pHitColModel;
    unsigned char m_nRadio1;
    unsigned char m_nRadio2;
    unsigned char m_nRace;
    uint8_t skip_0;
    short m_AudioPedType;
    short m_FirstVoice;
    short m_LastVoice;
    short m_NextVoice;

};


#endif //LIVERUSSIA_CPEDMODELINFO_H

//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_TIMEMODELINFO_H
#define LIVERUSSIA_TIMEMODELINFO_H


#include "AtomicModelInfo.h"
#include "TimeInfo.h"

class CTimeModelInfo : public CAtomicModelInfo {
public:
    CTimeInfo m_timeInfo;
};


#endif //LIVERUSSIA_TIMEMODELINFO_H

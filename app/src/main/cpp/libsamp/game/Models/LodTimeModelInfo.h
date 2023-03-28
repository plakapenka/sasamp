//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_LODTIMEMODELINFO_H
#define LIVERUSSIA_LODTIMEMODELINFO_H


#include "LodAtomicModelInfo.h"
#include "TimeInfo.h"

class CLodTimeModelInfo : public CLodAtomicModelInfo {
public:
    CTimeInfo m_lodTimeInfo;
};


#endif //LIVERUSSIA_LODTIMEMODELINFO_H

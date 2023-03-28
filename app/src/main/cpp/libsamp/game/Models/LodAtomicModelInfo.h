//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_LODATOMICMODELINFO_H
#define LIVERUSSIA_LODATOMICMODELINFO_H


#include "AtomicModelInfo.h"

class CLodAtomicModelInfo : public CAtomicModelInfo {
public:
    int16_t numChildren;
    int16_t numChildrenRendered;
};


#endif //LIVERUSSIA_LODATOMICMODELINFO_H

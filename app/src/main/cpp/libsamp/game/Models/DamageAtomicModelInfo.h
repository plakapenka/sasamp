//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_DAMAGEATOMICMODELINFO_H
#define LIVERUSSIA_DAMAGEATOMICMODELINFO_H


#include "../common.h"
#include "AtomicModelInfo.h"

class CDamageAtomicModelInfo : public CAtomicModelInfo {
public:
    RpAtomic* m_pDamagedAtomic;

    static bool& ms_bCreateDamagedVersion;
};


#endif //LIVERUSSIA_DAMAGEATOMICMODELINFO_H

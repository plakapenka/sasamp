//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_WEAPONMODELINFO_H
#define LIVERUSSIA_WEAPONMODELINFO_H


#include "ClumpModelInfo.h"
#include "../Enums/WeaponType.h"

class CWeaponModelInfo : public CClumpModelInfo {
public:
    eWeaponType m_weaponInfo;

};


#endif //LIVERUSSIA_WEAPONMODELINFO_H

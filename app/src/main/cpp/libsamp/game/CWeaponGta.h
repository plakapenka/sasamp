//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CWEAPONGTA_H
#define LIVERUSSIA_CWEAPONGTA_H

struct CWeaponGta
{
    uint32_t dwType;
    uint32_t dwState;
    uint32_t dwAmmoInClip;
    uint32_t dwAmmo;
    uint32_t m_nTimer;
    uint8_t m_bFirstPersonWeaponModeSelected;
    uint8_t m_bDontPlaceInHand;
    uint8_t skip[6];
};

#endif //LIVERUSSIA_CWEAPONGTA_H

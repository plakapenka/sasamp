#pragma once

#include "eWeaponType.h"

enum eWeaponSkill
{
	WEAPSKILL_POOR,
	WEAPSKILL_STD,
	WEAPSKILL_PRO,
	WEAPSKILL_COP
};

class CWeaponInfo
{
public:
    unsigned int   m_nWeaponFire; // see eWeaponFire
    float          m_fTargetRange; // max targeting range
    float          m_fWeaponRange; // absolute gun range / default melee attack range
    int            m_nModelId1; // modelinfo id
    int            m_nModelId2; // second modelinfo id


public:
    static CWeaponInfo* GetWeaponInfo(eWeaponType weaponType, signed char skill);
};

extern CWeaponInfo* aWeaponInfo;

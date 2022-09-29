#include "main.h"
#include "../util/patch.h"
#include "CWeaponInfo.h"

CWeaponInfo* aWeaponInfo = 0;

CWeaponInfo* CWeaponInfo::GetWeaponInfo(eWeaponType weaponType, signed char skill)
{
    return CPatch::CallFunction<CWeaponInfo*>(g_libGTASA + 0x56BD60 + 1, weaponType, skill);
}
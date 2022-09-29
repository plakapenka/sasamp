#include "main.h"
#include "CModelInfo.h"
#include "CWeaponInfo.h"
#include "CHud.h"

void GTASAInit()
{
    CModelInfo::ms_modelInfoPtrs = (CBaseModelInfo**)(g_libGTASA + 0x87BF48);
    aWeaponInfo = (CWeaponInfo*)(g_libGTASA + 0x9E4C88);
    CHud::Sprites = (CSprite2d**)(g_libGTASA + 0x8F0704);
}
//
// Created by plaka on 05.04.2023.
//

#include "Collision.h"
#include "../Core/Link.h"
#include "util/patch.h"

void CCollision::InjectHooks()
{
    CHook::Write(g_libGTASA + 0x00677A88, &CCollision::ms_colModelCache);
    CHook::Write(g_libGTASA + 0x006773BC, &CCollision::ms_collisionInMemory);
    CHook::Write(g_libGTASA + 0x006783D0, &CCollision::ms_iProcessLineNumCrossings);
}


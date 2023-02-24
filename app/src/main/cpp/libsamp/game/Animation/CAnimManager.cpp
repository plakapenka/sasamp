//
// Created by plaka on 21.02.2023.
//

#include "CAnimManager.h"


std::uint32_t CAnimManager::GetAnimationBlockIndex(char const* name)
{
    return ((std::uint32_t (*) (char const* name))(g_libGTASA + 0x0038DEE8 + 1))(name);
}

uintptr_t* CAnimManager::GetAnimation(unsigned int hash, uintptr_t const* ifp) {
    return ((uintptr_t* (*) (unsigned int, uintptr_t const*))(g_libGTASA + 0x0038DFC0 + 1))(hash, ifp);
}
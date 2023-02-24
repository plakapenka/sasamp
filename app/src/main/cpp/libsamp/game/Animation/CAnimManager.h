//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_CANIMMANAGER_H
#define LIVERUSSIA_CANIMMANAGER_H
#include "../../main.h"

class CAnimManager {
public:
    static std::uint32_t GetAnimationBlockIndex(char const* name);

    static uintptr_t *GetAnimation(unsigned int hash, const uintptr_t *ifp);
};


#endif //LIVERUSSIA_CANIMMANAGER_H

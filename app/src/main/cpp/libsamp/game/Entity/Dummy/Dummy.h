//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_DUMMY_H
#define LIVERUSSIA_DUMMY_H


#include "game/Entity/CEntityGta.h"

class CDummy : public CEntityGta {
public:
    CDummy();

   // ~CDummy() override = default;

    static void *operator new(size_t size);

    static void operator delete(void *obj);

};
static_assert(sizeof(CDummy) == 0x3c, "Invalid");

bool IsDummyPointerValid(CDummy* dummy);

#endif //LIVERUSSIA_DUMMY_H

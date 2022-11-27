//
// Created by plaka on 26.11.2022.
//

#ifndef LIVERUSSIA_CEDITOBJECT_H
#define LIVERUSSIA_CEDITOBJECT_H

#include "game/common.h"

#define INVALID_EDITED_SLOT -1

static class CEditobject {
public:
    CEditobject();
    static void StartEditAttachedObject(int slot);
    static bool isToggle;
    static int iEditedSlot;

    static void
    SendOnEditAttach(int response, int index, int modelid, int bone, _VECTOR offset, _VECTOR rot,
                     _VECTOR scale);
};


#endif //LIVERUSSIA_CEDITOBJECT_H

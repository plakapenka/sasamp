//
// Created by plaka on 26.11.2022.
//

#ifndef LIVERUSSIA_CEDITOBJECT_H
#define LIVERUSSIA_CEDITOBJECT_H

#include <jni.h>
#include "game/common.h"

#define INVALID_EDITED_SLOT -1

static class CEditobject {
public:
    static jobject thiz;
    static jclass clazz;
    static void StartEditAttachedObject(int slot);
    static bool bIsToggle;
    static int iEditedSlot;

    static void
    SendOnEditAttach(int response, int index, int modelid, int bone, CVector offset, CVector rot, CVector scale);
};


#endif //LIVERUSSIA_CEDITOBJECT_H

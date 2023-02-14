//
// Created by plaka on 11.02.2023.
//

#ifndef LIVERUSSIA_CSPEEDOMETR_H
#define LIVERUSSIA_CSPEEDOMETR_H


#include <jni.h>

static class CSpeedometr {

public:

    enum
    {
        BUTTON_ENGINE,
        BUTTON_LIGHT,
        BUTTON_TURN_LEFT,
        BUTTON_TURN_RIGHT,
        BUTTON_TURN_ALL
    };

    static bool bIsShow;
    static bool bIsNoNeedDraw;

    static void hide();

    static void show();

    static jclass clazz;

    static void update();

    static jobject thiz;

    static float fFuel;
    static int iMilliage;

    static void tempToggle(bool toggle);
};


#endif //LIVERUSSIA_CSPEEDOMETR_H

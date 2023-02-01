//
// Created by plaka on 01.02.2023.
//

#ifndef LIVERUSSIA_CNOTIFICATION_H
#define LIVERUSSIA_CNOTIFICATION_H


#include <jni.h>

static class CNotification {
public:
    static jobject thiz;

    static void show(int type, char *text, int duration, int actionId);

    static void hide();
};


#endif //LIVERUSSIA_CNOTIFICATION_H

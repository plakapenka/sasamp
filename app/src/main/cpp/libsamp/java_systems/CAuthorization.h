//
// Created by plaka on 20.02.2023.
//

#ifndef LIVERUSSIA_CAUTHORIZATION_H
#define LIVERUSSIA_CAUTHORIZATION_H


#include <jni.h>

static class CAuthorization {
    static jobject thiz;

public:
    static void show(char *nick, int id, bool ip_match, bool toggleAutoLogin, bool email_acvive);

    static void hide();

    static jclass clazz;
};


#endif //LIVERUSSIA_CAUTHORIZATION_H

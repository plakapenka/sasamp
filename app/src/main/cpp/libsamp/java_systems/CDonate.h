//
// Created by plaka on 16.02.2023.
//

#ifndef LIVERUSSIA_CDONATE_H
#define LIVERUSSIA_CDONATE_H


#include <jni.h>

class CDonate {
public:
    static jclass clazz;
    static jobject thiz;

    static void show(int balance);

    static void updateBalance(int balance);
};


#endif //LIVERUSSIA_CDONATE_H

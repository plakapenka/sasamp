//
// Created by plaka on 20.02.2023.
//

#ifndef LIVERUSSIA_CCHOOSESPAWN_H
#define LIVERUSSIA_CCHOOSESPAWN_H


#include <jni.h>

class CChooseSpawn {
    static jobject thiz;

public:

    static jclass clazz;

    static void hide();

    static void show(int organization, int station, int exit, int garage, int house);
};


#endif //LIVERUSSIA_CCHOOSESPAWN_H

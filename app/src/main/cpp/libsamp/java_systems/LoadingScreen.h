//
// Created by plaka on 26.02.2023.
//

#ifndef LIVERUSSIA_LOADINGSCREEN_H
#define LIVERUSSIA_LOADINGSCREEN_H


#include <jni.h>

class LoadingScreen {
    static jobject thiz;
public:
    static jclass clazz;

    static void linkedObj();

    static void updatePercent(int percent);

    static void updateText(char *text);

    static void hide();

    static void gtaLoadingScreenTick();
};


#endif //LIVERUSSIA_LOADINGSCREEN_H

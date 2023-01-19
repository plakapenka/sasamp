//
// Created by plaka on 19.01.2023.
//

#ifndef LIVERUSSIA_CBACCARAT_H
#define LIVERUSSIA_CBACCARAT_H


#include <jni.h>

static class CBaccarat {

public:
    static jobject thiz;
    static jclass clazz;

    static void
    updateBaccarat(int redCard, int yellowCard, int totalPlayers, int totalRed, int totalYellow,
                   int totalGreen, int time, int betType, int betSum);
};


#endif //LIVERUSSIA_CBACCARAT_H

//
// Created by plaka on 19.01.2023.
//

#ifndef LIVERUSSIA_CBACCARAT_H
#define LIVERUSSIA_CBACCARAT_H


#include <jni.h>

static class CBaccarat {

public:
    static const int BACCARAT_MAX_HISTORY = 9;
    static jobject thiz;
    static jclass clazz;
    static bool bIsShow;

    static void
    updateBaccarat(int redCard, int yellowCard, int totalPlayers, int totalRed, int totalYellow,
                   int totalGreen, int time, int betType, int betSum, int winner, int balance);

    static void tempToggle(bool toggle);

    static void updateLastWins(uint8_t* lastwins);

    static void hide();
};


#endif //LIVERUSSIA_CBACCARAT_H

//
// Created by plaka on 31.01.2023.
//

#ifndef LIVERUSSIA_CDAILYREWARD_H
#define LIVERUSSIA_CDAILYREWARD_H


#include <jni.h>


static class CDailyReward {
public:
    static bool bIsShow;
    static jobject thiz;

    static void show(int day, int second);
};


#endif //LIVERUSSIA_CDAILYREWARD_H

//
// Created by plaka on 05.02.2023.
//

#ifndef LIVERUSSIA_CTECHINSPECT_H
#define LIVERUSSIA_CTECHINSPECT_H


#include <jni.h>

static class CTechInspect {
public:
    static jobject thiz;
    static bool bIsShow;

    static void show(char *name, int gen, int candl, int brake, int starter, int nozzles, int price, int carid);
};


#endif //LIVERUSSIA_CTECHINSPECT_H

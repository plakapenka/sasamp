#pragma once

#include <iostream>
#include <jni.h>

static class CTab {
public:
    static void toggle();
    static void update();
    static bool bIsShow;
    static jobject thiz;

    static void show();

    static void setStat(int id, char *name, int score, int ping);
};



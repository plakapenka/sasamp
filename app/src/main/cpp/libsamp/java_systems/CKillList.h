//
// Created by plaka on 21.01.2023.
//

#ifndef LIVERUSSIA_CKILLLIST_H
#define LIVERUSSIA_CKILLLIST_H


#include <jni.h>

static class CKillList {

public:
    static jobject thiz;

    static void addMessage(unsigned short killer, unsigned short killee, int reason, int team);
};


#endif //LIVERUSSIA_CKILLLIST_H

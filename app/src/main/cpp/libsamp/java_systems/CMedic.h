//
// Created by plaka on 14.01.2023.
//

#ifndef LIVERUSSIA_CMEDIC_H
#define LIVERUSSIA_CMEDIC_H

#include <jni.h>
#include <string>

class CMedic {

public:
    static jobject thiz;
    static bool bIsShow;

    static void showPreDeath(char* nick, int id);

    //void hidePreDeath();

    static void showMedGame(char *nick);

    static void hide();
};


#endif //LIVERUSSIA_CMEDIC_H

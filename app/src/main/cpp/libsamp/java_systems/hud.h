//
// Created by plaka on 28.10.2022.
//
#pragma once
#include "../util/CJavaWrapper.h"


#ifndef LIVERUSSIA_HUD_H
#define LIVERUSSIA_HUD_H



class CHUD {
public:
    void InitServerLogo(int i);

    void ToggleAll(bool toggle, bool withchat = false, bool anyway = false);
    void UpdateHudInfo();
    CHUD();
    bool isHudToggle = false;
    int hudhideCount = 0;
    int localMoney = 0;
};


#endif //LIVERUSSIA_HUD_H

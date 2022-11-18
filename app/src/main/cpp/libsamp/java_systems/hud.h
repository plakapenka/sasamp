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

    void ToggleAll(bool toggle, bool withchat = false);
    void UpdateHudInfo();
    void ToggleEnterPassengerButton(bool toggle);
    void ToggleEnterExitVehicleButton(bool toggle);
    void ToggleLockVehicleButton(bool toggle);

    int GetScreenSize(bool isWidth);
    CHUD();
    bool isHudToggle = false;
    int localMoney = 0;
    bool isEnterPassengerButtOn = false;
    bool isEnterExitVehicleButtonOn = false;
    bool isLockVehicleButtonOn = false;
    bool isHornButtonOn = false;

    void UpdateWanted();

    void UpdateMoney();

    void ToggleHornButton(bool toggle);

    void UpdateOpgWarLayout(int time, int attack_score, int def_score);

    bool isMafia_war_layout_active = false;
};


#endif //LIVERUSSIA_HUD_H

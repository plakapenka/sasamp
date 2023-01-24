//
// Created by plaka on 28.10.2022.
//
#pragma once
#include "../util/CJavaWrapper.h"
#include "game/common.h"


#ifndef LIVERUSSIA_HUD_H
#define LIVERUSSIA_HUD_H



static class CHUD {
public:

    static void toggleAll(bool toggle, bool withchat = false);
    static void UpdateHudInfo();
    static void togglePassengerButton(bool toggle);
    static void toggleEnterExitButton(bool toggle);
    static void toggleLockButton(bool toggle);

    CHUD();
    static bool bIsShow;
    static int  iLocalMoney;
    static bool bIsShowPassengerButt;
    static bool bIsShowEnterExitButt;
    static bool bIsShowLockButt;
    static bool bIsShowHornButt;
    static bool bIsShowChat;
    static bool bIsTouchCameraButt;
    static bool bIsCamEditGui;
    static jobject thiz;

    static void UpdateWanted();

    static void UpdateMoney();

    static void toggleHornButton(bool toggle);

    static void updateOpgWarLayout(int time, int attack_score, int def_score);

    static bool bIsShowMafiaWar;

    static CVector2DFloat radarBgPos1;
    static CVector2DFloat radarBgPos2;
    static CVector2DFloat radarPos;

    static void AddChatMessage(const char msg[]);

    static void SetChatInput(const char ch[]);

    static void ToggleChatInput(bool toggle);

    static void ToggleHpText(bool toggle);

    static void ChangeChatHeight(int height);

    void ToggleChat(bool toggle);

    static void ChangeChatTextSize(int size);

    static void UpdateSalary(int salary, int lvl, float exp);

    static int iWantedLevel;

    static void addGiveDamageNotify(PLAYERID Id, int weaponId, float damage);
    static PLAYERID lastGiveDamagePlayerId;
    static float fLastGiveDamage;

    static void addTakeDamageNotify(char *nick, int weaponId, float damage);
};


#endif //LIVERUSSIA_HUD_H

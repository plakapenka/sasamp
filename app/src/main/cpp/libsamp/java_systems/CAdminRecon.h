//
// Created by plaka on 10.01.2023.
//

#ifndef LIVERUSSIA_CADMINRECON_H
#define LIVERUSSIA_CADMINRECON_H


#include "game/common.h"

static class CAdminRecon {

public:
    static bool bIsToggle;
    static PLAYERID iPlayerID;

    static void show(int targetID);

    static void hide();

    enum Buttons{
        EXIT_BUTTON,
        STATS_BUTTON,
        FREEZE_BUTTON,
        UNFREEZE_BUTTON,
        SPAWN_BUTTON,
        SLAP_BUTTON,
        REFRESH_BUTTON,
        MUTE_BUTTON,
        JAIL_BUTTON,
        KICK_BUTTON,
        BAN_BUTTON,
        WARN_BUTTON,
        NEXT_BUTTON,
        PREV_BUTTON
    };
};


#endif //LIVERUSSIA_CADMINRECON_H

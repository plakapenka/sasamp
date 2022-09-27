#pragma once

#include "../vendor/imgui/imgui.h"
#include "../game/RW/RenderWare.h"

enum typeState
{
    OFF = 0,
    ON
};

class CInterface
{

public:
    CInterface();
    ~CInterface();

    void RenderSpeedometer();

    static void SetupKeyboardStyle();

private:
    int m_iCurrentHudID;
    int m_iWantedLevel;

    int m_iCurrentVolumePlayer;

    bool m_bRenderHud;
    bool m_bRenderMenu;

    ImFont *speedFont;

    struct RwTexture *engineTex0;
    struct RwTexture *engineTex1;
    struct RwTexture *lockTex0;
    struct RwTexture *lockTex1;
    struct RwTexture *lightTex0;
    struct RwTexture *lightTex1;
    struct RwTexture *beltTex0;
    struct RwTexture *beltTex1;
    struct RwTexture *speedometer0;
    struct RwTexture *speedometer1;
    struct RwTexture *turnleft0;
    struct RwTexture *turnleft1;
    struct RwTexture *turnright0;
    struct RwTexture *turnright1;

    bool bEngine, bLight, bLock, bBelt, bEmergency, bTurnleft, bTurnright;
    int iMileage;
};

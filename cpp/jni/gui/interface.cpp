#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"
#include "gui.h"
#include "interface.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;

static int m_iContentId;

CInterface::CInterface()
{
    m_iCurrentHudID = 0; // pSettings->Get().iHudStyle;
    m_iWantedLevel = 0;
    m_iContentId = 0;

    m_bRenderHud = true;
    m_bRenderMenu = false;
    iMileage = 0;
    speedFont = pGUI->LoadFont("visby-round-cf-extra-bold.ttf", 25);

    bEngine = false;
    bLight = false;
    bLock = false;
    bBelt = false;
    bEmergency = false;
    bTurnleft = false;
    bTurnright = false;
}

CInterface::~CInterface() = default;

void CInterface::RenderSpeedometer()
{

    if (!pGame || !pNetGame || !pGame->FindPlayerPed()->IsInVehicle())
        return;
    if (pGame->FindPlayerPed()->IsAPassenger())
        return;

    ImGuiIO &io = ImGui::GetIO();

    ImGui::Begin("Speedometer Flame", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

    

    ImGui::SetWindowSize(ImVec2(pGUI->ScaleY(900), pGUI->ScaleY(400)));
    ImGui::SetWindowPos(ImVec2(
        io.DisplaySize.x / 2 - ImGui::GetWindowWidth() / 2,
        io.DisplaySize.y - ImGui::GetWindowHeight() * 0.97f));
    ImGui::End();
}
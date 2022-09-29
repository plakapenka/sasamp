#include "../main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "CHUD.h"
#include "../util/patch.h"
#include <string>

// GTASA
#include "../gtasa/CModelInfo.h"
#include "../gtasa/CWeaponInfo.h"
#include "../gtasa/CHud.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

bool CHUD::m_bShow = true;
ImFont *CHUD::speedFont = 0;

RwTexture *CHUD::hud_1_background, *CHUD::hud_1_fist, *CHUD::hud_1_star;
RwTexture *CHUD::hud_2_radar_background, *CHUD::hud_2_cashicon, *CHUD::hud_2_star, *CHUD::hud_2_health, *CHUD::hud_2_armour, *CHUD::hud_2_eat;
HUD_SELECT CHUD::m_iHud = CHUD_ONE;

RwTexture *CHUD::speed_1_fuel = 0;
RwTexture *CHUD::speed_1_health = 0;
RwTexture *CHUD::speed_1_engine[2];
RwTexture *CHUD::speed_1_light[2];
RwTexture *CHUD::speed_1_lock[2];

uint16_t CHUD::iHungry, CHUD::iVehFuel, CHUD::iVehLight, CHUD::iVehEngine, CHUD::iVehLock;

void CHUD::Initialise()
{
    // Disable();
    hud_1_background = (RwTexture *)LoadTextureFromDB("samp", "hud_1_background");
    hud_1_fist = (RwTexture *)LoadTextureFromDB("samp", "hud_1_fist");
    hud_1_star = (RwTexture *)LoadTextureFromDB("samp", "hud_1_star");

    speedFont = pGUI->LoadFont("Montserrat-Bold.ttf", 30);

    speed_1_fuel = (RwTexture *)LoadTextureFromDB("samp", "speed_1_fuel");
    speed_1_health = (RwTexture *)LoadTextureFromDB("samp", "speed_1_health");

    speed_1_light[0] = (RwTexture *)LoadTextureFromDB("samp", "speed_1_light_off");
    speed_1_light[1] = (RwTexture *)LoadTextureFromDB("samp", "speed_1_light_on");

    speed_1_engine[0] = (RwTexture *)LoadTextureFromDB("samp", "speed_1_engine_off");
    speed_1_engine[1] = (RwTexture *)LoadTextureFromDB("samp", "speed_1_engine_on");

    speed_1_lock[0] = (RwTexture *)LoadTextureFromDB("samp", "speed_1_lock_off");
    speed_1_lock[1] = (RwTexture *)LoadTextureFromDB("samp", "speed_1_lock_on");

    hud_2_radar_background = (RwTexture *)LoadTextureFromDB("samp", "hud_2_radar_background");
    hud_2_cashicon = (RwTexture *)LoadTextureFromDB("samp", "hud_2_cashicon");
    hud_2_star = (RwTexture *)LoadTextureFromDB("samp", "hud_2_star");
    hud_2_health = (RwTexture *)LoadTextureFromDB("samp", "hud_2_health");
    hud_2_armour = (RwTexture *)LoadTextureFromDB("samp", "hud_2_armour");
    hud_2_eat = (RwTexture *)LoadTextureFromDB("samp", "hud_2_eat");

    // CHud::Sprites[HUDSPRITE_FIST]->m_pRwTexture = 0;
}

#define CHUD_SIZE_FACTOR 0.8
#define CHUD_SPEED_FACTOR 1.3
void CHUD::EditFist(CRect *rect)
{
    switch (m_iHud)
    {
    case CHUD_ONE:
        rect->left = pGUI->ScaleX(1920) - pGUI->ScaleY(550 * CHUD_SIZE_FACTOR) + pGUI->ScaleY(57 * CHUD_SIZE_FACTOR);
        rect->right = rect->left + pGUI->ScaleY(200 * CHUD_SIZE_FACTOR);

        rect->top = pGUI->ScaleY(-100 * CHUD_SIZE_FACTOR) + pGUI->ScaleY(175 * CHUD_SIZE_FACTOR);
        rect->bottom = rect->top + pGUI->ScaleY(200 * CHUD_SIZE_FACTOR);

        break;
    }
}

void CHUD::EditRadar(CRect *rect)
{
    // posX posY ScaleX ScaleY
    switch (m_iHud)
    {
    case CHUD_TWO:
        rect->left = 60.0f;
        rect->bottom = 70.0f;
        rect->right = 45.0f;
        rect->top = 45.0f;
        break;

    default:
        break;
    }
}

void CHUD::Select(HUD_SELECT hudNum)
{
    m_iHud = hudNum;
    switch (hudNum)
    {
    case CHUD_NOP:
        CPatch::WriteMemory(g_libGTASA + 0x27E24E, "\xFF\xF7\x2B\xFB", 4); // DrawWanted
        CPatch::WriteMemory(g_libGTASA + 0x27E1E8, "\xFF\xF7\xD6\xF9", 4); // RenderBreathBar
        CPatch::WriteMemory(g_libGTASA + 0x27E1AE, "\xFF\xF7\xA1\xFA", 4); // RenderArmorBar
        CPatch::WriteMemory(g_libGTASA + 0x27E188, "\xFF\xF7\x20\xF9", 4); // RenderHealthBar
        CPatch::WriteMemory(g_libGTASA + 0x27E158, "\xB7\xF2\x2C\xF9", 4); // PrintMoney
        CPatch::WriteMemory(g_libGTASA + 0x27E056, "\xB7\xF2\xAD\xF9", 4); // PrintTime
        break;

    case CHUD_TWO:
    case CHUD_ONE:
        CPatch::NOP(g_libGTASA + 0x27E24E, 2); // DrawWanted
        CPatch::NOP(g_libGTASA + 0x27E1E8, 2); // RenderBreathBar
        CPatch::NOP(g_libGTASA + 0x27E1AE, 2); // RenderArmorBar
        CPatch::NOP(g_libGTASA + 0x27E188, 2); // RenderHealthBar
        CPatch::NOP(g_libGTASA + 0x27E158, 2); // PrintMoney
        CPatch::NOP(g_libGTASA + 0x27E056, 2); // PrintTime
        break;
    }
}

void CHUD::Render()
{
    if (!IsEnabled())
        return;

    if (m_iHud == CHUD_ONE)
    { //*(uint8_t*)(g_libGTASA+0x7165E8) = 0; //orig hud off

        ImVec2 vecBackground_1(pGUI->ScaleX(1920) - pGUI->ScaleY(550 * CHUD_SIZE_FACTOR), pGUI->ScaleY(-100 * CHUD_SIZE_FACTOR));
        ImVec2 vecBackground_2(vecBackground_1.x + pGUI->ScaleY(550 * CHUD_SIZE_FACTOR), vecBackground_1.y + pGUI->ScaleY(550 * CHUD_SIZE_FACTOR));

        ImGui::GetBackgroundDrawList()->AddImage(hud_1_background->raster, vecBackground_1, vecBackground_2);

        ImVec2 vecFist_1(vecBackground_1.x + pGUI->ScaleY(58 * CHUD_SIZE_FACTOR), vecBackground_1.y + pGUI->ScaleY(175 * CHUD_SIZE_FACTOR));
        ImVec2 vecFist_2(vecFist_1.x + pGUI->ScaleY(200 * CHUD_SIZE_FACTOR), vecFist_1.y + pGUI->ScaleY(200 * CHUD_SIZE_FACTOR));
        PED_TYPE *a2 = GamePool_FindPlayerPed();
        CWeaponInfo *v9 = CWeaponInfo::GetWeaponInfo((eWeaponType)a2->WeaponSlots[a2->byteCurWeaponSlot].dwType, WEAPSKILL_STD);
        if (v9->m_nModelId1 <= GTASA_WEAPON_UNARMED)
        {
            ImGui::GetBackgroundDrawList()->AddImage(hud_1_fist->raster, vecFist_1, vecFist_2);
        }
        else
        {
            char buff[0x40];
            char buff2[0x40];
            strcpy(buff, CModelInfo::ms_modelInfoPtrs[v9->m_nModelId1]->modelName);
            // Log("ICON: %s", buff);
            sprintf(buff2, "%sicon", buff);
            RwTexture *icon = (RwTexture *)LoadTextureFromDB("gta3", buff2);
            ImGui::GetBackgroundDrawList()->AddImage(icon->raster, vecFist_1, vecFist_2);
        }

        static float X = 297.0f;
        static float Y = 250.0f;
        ImVec2 vecHealthBar_1(vecBackground_1.x + pGUI->ScaleY(353.0f * CHUD_SIZE_FACTOR), vecBackground_1.y + pGUI->ScaleY(254.0f * CHUD_SIZE_FACTOR));

        float fHealth = pGame->FindPlayerPed()->GetHealth();
        if (fHealth > 100.0f)
            fHealth = 100.0f;
        float fWidthHealth = (pGUI->ScaleY(148 * CHUD_SIZE_FACTOR) / 100.0f) * fHealth;

        ImVec2 vecHealthBar_2(vecHealthBar_1.x + fWidthHealth, vecHealthBar_1.y + pGUI->ScaleY(10 * CHUD_SIZE_FACTOR));

        // ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(vecHealthBar_1, vecHealthBar_2, ImColor(0x9F, 0x56, 0xCB, 0xFF), ImColor(0xF9, 0x88, 0xE7, 0xFF), ImColor(0xF9, 0x88, 0xE7, 0xFF), ImColor(0x9F, 0x56, 0xCB, 0xFF));
        ImGui::GetBackgroundDrawList()->AddRectFilled(vecHealthBar_1, vecHealthBar_2, ImColor(0xFF, 0x00, 0x00, 255), 50);

        ImVec2 vecArmorBar_1(vecHealthBar_1.x, vecHealthBar_2.y + pGUI->ScaleY(25.0f * CHUD_SIZE_FACTOR));

        float fArmor = pGame->FindPlayerPed()->GetArmour();
        if (fArmor > 100.0f)
            fArmor = 100.0f;
        float fWidthArmor = (pGUI->ScaleY(120 * CHUD_SIZE_FACTOR) / 100.0f) * fArmor;

        ImVec2 vecArmorBar_2(vecArmorBar_1.x + fWidthArmor, vecArmorBar_1.y + pGUI->ScaleY(10 * CHUD_SIZE_FACTOR));

        // ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(vecArmorBar_1, vecArmorBar_2, ImColor(0xAC, 0xCB, 0xEE, 0xFF), ImColor(0xE7, 0xF0, 0xFD, 0xFF), ImColor(0xAC, 0xCB, 0xEE, 0xFF),ImColor(0xE7, 0xF0, 0xFD, 0xFF));
        ImGui::GetBackgroundDrawList()->AddRectFilled(vecArmorBar_1, vecArmorBar_2, ImColor(0xAC, 0xCB, 0xEE, 0xFF), 50);

        int iMoneyCount = pGame->GetLocalMoney();
        char buff[0x50];
        sprintf(buff, "%d$", iMoneyCount);
        ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), pGUI->ScaleY(23), ImVec2(vecBackground_1.x + pGUI->ScaleY(420 * CHUD_SIZE_FACTOR) - (ImGui::GetFont()->CalcTextSizeA(pGUI->ScaleY(23), FLT_MAX, -1.0f, buff).x / 2), vecBackground_1.y + pGUI->ScaleY(187 * CHUD_SIZE_FACTOR)), IM_COL32_WHITE, buff);

        for (int i = 0; i < pGame->GetWantedLevel(); i++)
        {
            ImVec2 vecWanted_1(vecArmorBar_1.x - pGUI->ScaleY(53.5f * CHUD_SIZE_FACTOR) + (pGUI->ScaleY(34 * CHUD_SIZE_FACTOR) * i), vecArmorBar_2.y + pGUI->ScaleY(32 * CHUD_SIZE_FACTOR));
            ImVec2 vecWanted_2(vecWanted_1.x + pGUI->ScaleY(34 * CHUD_SIZE_FACTOR), vecWanted_1.y + pGUI->ScaleY(35 * CHUD_SIZE_FACTOR));

            ImGui::GetBackgroundDrawList()->AddImage(hud_1_star->raster, vecWanted_1, vecWanted_2);
        }
    }
    else if (m_iHud == CHUD_TWO)
    {

        uintptr_t dword_6580C8 = *(uintptr_t *)(g_libGTASA + 0x6580C8);

        float v4 = *(float *)(dword_6580C8 + 32);
        float v6 = *(float *)(dword_6580C8 + 40);
        float v7 = *(float *)(dword_6580C8 + 36);
        float v8 = *(float *)(dword_6580C8 + 44);

        float v11 = (float)(v4 + v6) * 0.5;
        float v10 = fabsf(v6 - v4) * 0.5;
        float v12 = fabsf(v7 - v8) * 0.5;
        float v13 = (float)(v7 + v8) * 0.5;

        float v22 = v10 * 1.02;
        float v23 = v12 * 1.02;

        float v44 = v11 - v22;
        float v45 = v13;
        float v46 = v11;
        float v47 = v13 - v23;

        /*
            radar sprite1

            y2 - v47

            y1 - v45   x1 - v44    x2 - v46

        */

        ImVec2 generalPos(v46, v45);
        float fRadius = (v46 - v44) * 1.07;

        ImVec2 vecRadar_Background_1(v44 * 1.5, (v45 + ((v45 - v47) / 2)));
        ImVec2 vecRadar_Background_2((vecRadar_Background_1.x + ((v46 - v44 * 1.5) * 2)), (v45 + (v45 - v47)));

        ImGui::GetOverlayDrawList()->AddImage(hud_2_radar_background->raster, vecRadar_Background_1, vecRadar_Background_2);

        // background one
        float a0 = -0.7854 / 10 * (40 - 7.6);
        float a1 = -0.7854 / 10 * (40 + 7.6);
        ImGui::GetOverlayDrawList()->PathArcTo(generalPos, fRadius, a0, a1, 50);
        ImGui::GetOverlayDrawList()->PathStroke(ImColor(38, 38, 38, 200), false, pGUI->ScaleY(15.0f));

        a0 = 0.7854 / 10 * 7.6;
        a1 = -0.7854 / 10 * 7.6;
        ImGui::GetOverlayDrawList()->PathArcTo(generalPos, fRadius, a0, a1, 50);
        ImGui::GetOverlayDrawList()->PathStroke(ImColor(38, 38, 38, 200), false, pGUI->ScaleY(15.0f));

        a0 = 0.7854 / 10 * 7.2;
        a1 = -0.7854 / 10 * 7.2;
        ImGui::GetOverlayDrawList()->PathArcTo(generalPos, fRadius + pGUI->ScaleY(18.0f), a0, a1, 50);
        ImGui::GetOverlayDrawList()->PathStroke(ImColor(38, 38, 38, 200), false, pGUI->ScaleY(15.0f));

        float fHungry = iHungry;
        if (fHungry > 100.0f)
            fHungry = 100.0f;

        a0 = 0.7854 / 10 * (7.2 - (7.2 * 2) / 100.0f * fHungry);
        a1 = 0.7854 / 10 * 7.2;
        ImGui::GetOverlayDrawList()->PathArcTo(generalPos, fRadius + pGUI->ScaleY(18.0f), a0, a1, 50);
        ImGui::GetOverlayDrawList()->PathStroke(ImColor(255, 205, 179, 255), false, pGUI->ScaleY(15.0f));

        float fArmour = pGame->FindPlayerPed()->GetArmour();
        if (fArmour > 100.0f)
            fArmour = 100.0f;

        a1 = -0.7854 / 10 * (40 + 7.6 - ((7.6 * 2) / 100.0f * fArmour));
        a0 = -0.7854 / 10 * (40 + 7.6);
        ImGui::GetOverlayDrawList()->PathArcTo(generalPos, fRadius, a0, a1, 50);
        ImGui::GetOverlayDrawList()->PathStroke(ImColor(179, 215, 243, 255), false, pGUI->ScaleY(15.0f));

        float fHealth = pGame->FindPlayerPed()->GetHealth();
        if (fHealth > 100.0f)
            fHealth = 100.0f;

        a0 = 0.7854 / 10 * (7.6 - (7.6 * 2) / 100.0f * fHealth);
        a1 = 0.7854 / 10 * 7.6;
        ImGui::GetOverlayDrawList()->PathArcTo(generalPos, fRadius, a0, a1, 50);
        ImGui::GetOverlayDrawList()->PathStroke(ImColor(0xFF, 0x00, 0x00, 255), false, pGUI->ScaleY(15.0f));

        uint32_t iCountMoney = pGame->GetLocalMoney();
        ImVec2 vecMoney_1(generalPos.x + (fRadius * 0.2f), generalPos.y + (fRadius * 0.54f));
        char buff[0x30];
        sprintf(buff, "%d", iCountMoney);
        vecMoney_1.x = vecMoney_1.x - (ImGui::GetFont()->CalcTextSizeA(fRadius / 9.0, FLT_MAX, -1.0, buff).x / 2);

        ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fRadius / 9.0, vecMoney_1, IM_COL32_WHITE, buff);

        ImVec2 vecCashIcon_1(generalPos.x - (fRadius * 0.3), generalPos.y + (fRadius * 0.5f));
        ImVec2 vecCashIcon_2(vecCashIcon_1.x + (fRadius / 5.0), vecCashIcon_1.y + (fRadius / 5.0));

        ImGui::GetOverlayDrawList()->AddImage(hud_2_cashicon->raster, vecCashIcon_1, vecCashIcon_2);

        for (int i = 0; i < pGame->GetWantedLevel(); i++)
        {
            ImVec2 vecWanted_1(generalPos.x - (fRadius * 0.37) + (i * (fRadius / 8.0)), generalPos.y + (fRadius * 0.65f));
            ImVec2 vecWanted_2(vecWanted_1.x + (fRadius / 8.0), vecWanted_1.y + (fRadius / 8.0));

            ImGui::GetBackgroundDrawList()->AddImage(hud_2_star->raster, vecWanted_1, vecWanted_2);
        }

        ImVec2 vecIcon_1(generalPos.x - (fRadius * 0.87), generalPos.y + (fRadius * 0.64f));
        ImVec2 vecIcon_2(vecIcon_1.x + (fRadius / 8.0), vecIcon_1.y + (fRadius / 8.0));
        ImGui::GetOverlayDrawList()->AddImage(hud_2_armour->raster, vecIcon_1, vecIcon_2);

        vecIcon_1 = ImVec2(generalPos.x + (fRadius * 0.87) - (fRadius / 8.0), vecIcon_1.y);
        vecIcon_2 = ImVec2(vecIcon_1.x + (fRadius / 8.0), vecIcon_1.y + (fRadius / 8.0));
        ImGui::GetOverlayDrawList()->AddImage(hud_2_health->raster, vecIcon_1, vecIcon_2);

        vecIcon_1 = ImVec2(generalPos.x + (fRadius * 1.01) - (fRadius / 8.0), vecIcon_1.y);
        vecIcon_2 = ImVec2(vecIcon_1.x + (fRadius / 8.0), vecIcon_1.y + (fRadius / 8.0));
        ImGui::GetOverlayDrawList()->AddImage(hud_2_eat->raster, vecIcon_1, vecIcon_2);
    }
    if (!pGame->FindPlayerPed()->IsInVehicle())
        return;

    ImVec2 generalPos(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y * 0.9);

    // background one
    float a0 = -0.7854 / 10 * 44;
    float a1 = 0.7854 / 10 * 4;
    //   ImGui::GetOverlayDrawList()->PathArcTo(generalPos, pGUI->ScaleY(140), a0, a1, 50);
    //    ImGui::GetOverlayDrawList()->PathStroke(ImColor(38, 38, 38, 200), false, pGUI->ScaleY(10.0f));

    for (int i = 0; i < 22; i++)
    {
        a0 = -IM_PI / 4.0f / 10.0 * (38.0 - (1.7015 * i));
        a1 = -IM_PI / 4.0f / 10.0 * (38.0 - (1.7015 * i) - 0.4);

        //   ImGui::GetOverlayDrawList()->PathArcTo(generalPos, pGUI->ScaleY(110), a0, a1, 50);
        //     ImGui::GetOverlayDrawList()->PathStroke(ImColor(38, 38, 38, 200), false, pGUI->ScaleY(20.0f));
    }

    // speed
    int i_speed = 0;
    double d_speed = 0.0;
    CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pGame->FindPlayerPed()->GetGtaVehicle()));
    if (pVehicle)
    {
        VECTOR vecMoveSpeed;
        pVehicle->GetMoveSpeedVector(&vecMoveSpeed);
        i_speed = sqrt((vecMoveSpeed.X * vecMoveSpeed.X) + (vecMoveSpeed.Y * vecMoveSpeed.Y) + (vecMoveSpeed.Z * vecMoveSpeed.Z)) * 183;
    }

    char speedText[10];
    sprintf(speedText, "%i", i_speed);
    if (i_speed >= 300)
        i_speed = 300;

    a0 = -0.7854 / 10 * 44;
    a1 = a0 + (0.7854 / 10 * 48 / 300 * i_speed);
    ImGui::GetOverlayDrawList()->PathArcTo(generalPos, pGUI->ScaleY(140), a0, a1, 50);
    ImGui::GetOverlayDrawList()->PathStroke(ImColor(0, 150, 0, 200), false, pGUI->ScaleY(10.0f));

    ImGui::GetOverlayDrawList()->AddText(speedFont, pGUI->ScaleY(60), ImVec2(generalPos.x - (speedFont->CalcTextSizeA(pGUI->ScaleY(60), FLT_MAX, -1.0f, speedText).x / 2), generalPos.y - pGUI->ScaleY(80)), IM_COL32_WHITE, speedText);
    ImGui::GetOverlayDrawList()->AddText(speedFont, pGUI->ScaleY(20), ImVec2(generalPos.x - (speedFont->CalcTextSizeA(pGUI->ScaleY(20), FLT_MAX, -1.0f, "км/ч").x / 2), generalPos.y - pGUI->ScaleY(25)), IM_COL32_WHITE, "км/ч");

    // background two
    a0 = -0.7854 / 10 * 44;
    a1 = 0.7854 / 10 * 4;
    ImGui::GetOverlayDrawList()->PathArcTo(generalPos, pGUI->ScaleY(140 - (12)), a0, a1, 50);
    ImGui::GetOverlayDrawList()->PathStroke(ImColor(38, 38, 38, 200), false, pGUI->ScaleY(10.0f));

    float fFuel = iVehFuel;
    a0 = -0.7854 / 10 * 44;
    a1 = a0 + (0.7854 / 10 * 24 / 100 * fFuel);
    ImGui::GetOverlayDrawList()->PathArcTo(generalPos, pGUI->ScaleY(140 - (12)), a0, a1, 50);
    ImGui::GetOverlayDrawList()->PathStroke(ImColor(245, 170, 66, 255), false, pGUI->ScaleY(10.0f));

    float iHealth = pGame->FindPlayerPed()->GetGtaVehicle()->fHealth;
    a0 = 0.7854 / 10 * 4;
    a1 = a0 - (0.7854 / 10 * 24 / 1000 * iHealth);
    ImGui::GetOverlayDrawList()->PathArcTo(generalPos, pGUI->ScaleY(140 - (12)), a0, a1, 50);
    ImGui::GetOverlayDrawList()->PathStroke(ImColor(245, 0, 0, 255), false, pGUI->ScaleY(10.0f));

    ImGui::GetOverlayDrawList()->AddImage(speed_1_fuel->raster, ImVec2(generalPos.x - pGUI->ScaleY(120), generalPos.y - pGUI->ScaleY(5)), ImVec2(generalPos.x - pGUI->ScaleY(120) + pGUI->ScaleY(40), generalPos.y - pGUI->ScaleY(5) + pGUI->ScaleY(40)));
    ImGui::GetOverlayDrawList()->AddImage(speed_1_health->raster, ImVec2(generalPos.x + pGUI->ScaleY(120 - 40), generalPos.y - pGUI->ScaleY(5)), ImVec2(generalPos.x + pGUI->ScaleY(120 - 40) + pGUI->ScaleY(40), generalPos.y - pGUI->ScaleY(5) + pGUI->ScaleY(40)));

    ImGui::GetOverlayDrawList()->AddImage(speed_1_light[iVehLight]->raster, ImVec2(generalPos.x - pGUI->ScaleY(20), generalPos.y + pGUI->ScaleY(35)), ImVec2(generalPos.x - pGUI->ScaleY(20) + pGUI->ScaleY(40), generalPos.y + pGUI->ScaleY(35) + pGUI->ScaleY(40)));
    ImGui::GetOverlayDrawList()->AddImage(speed_1_engine[iVehEngine]->raster, ImVec2(generalPos.x - pGUI->ScaleY(60), generalPos.y + pGUI->ScaleY(15)), ImVec2(generalPos.x - pGUI->ScaleY(60) + pGUI->ScaleY(40), generalPos.y + pGUI->ScaleY(15) + pGUI->ScaleY(40)));
    ImGui::GetOverlayDrawList()->AddImage(speed_1_lock[iVehLock]->raster, ImVec2(generalPos.x + pGUI->ScaleY(20), generalPos.y + pGUI->ScaleY(15)), ImVec2(generalPos.x + pGUI->ScaleY(20) + pGUI->ScaleY(40), generalPos.y + pGUI->ScaleY(15) + pGUI->ScaleY(40)));
}
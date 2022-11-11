//
// Created by plaka on 28.10.2022.
//

#include "hud.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

extern CJavaWrapper *g_pJavaWrapper;
extern CGame *pGame;
extern CHUD *pHud;
extern CGUI* pGUI;

jobject jHudManager;
jmethodID jUpdateHudInfo;

CHUD::CHUD()
{
    // LoadSettings(nullptr);
}

void CHUD::InitServerLogo(int serverID) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID InitServerLogo = env->GetMethodID(clazz, "InitServerLogo", "(I)V");

    env->CallVoidMethod(jHudManager, InitServerLogo, serverID);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_HudInit(JNIEnv *env, jobject thiz) {

    jHudManager = env->NewGlobalRef(thiz);
    jUpdateHudInfo = env->GetMethodID(env->GetObjectClass(thiz), "UpdateHudInfo", "(IIIIII)V");
}
extern bool showSpeedometr;
void CHUD::ToggleAll(bool toggle, bool withchat)
{
    if(toggle == isHudToggle)
    {
        return;
    }
    if(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->IsInVehicle())
    {
        if(!toggle)
        {
            showSpeedometr = false;
            g_pJavaWrapper->HideSpeed();
        }
        else
        {
            showSpeedometr = true;
            g_pJavaWrapper->ShowSpeed();
        }
    }

    isHudToggle = toggle;
    pGame->DisplayHUD(toggle);


    if(withchat || toggle)pGame->ToggleHUDElement(HUD_ELEMENT_CHAT, toggle);
    pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, toggle);
    //pGame->DisplayWidgets(toggle); ?? не работает тоже
    pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(toggle, true);
    pGame->ToggleHUDElement(HUD_ELEMENT_FPS, toggle);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(jHudManager);

    jmethodID ToggleAll = env->GetMethodID(clazz, "ToggleAll", "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleAll, toggle);

  //  int radarPosX = (int)pHud->GetScreenSize(true)/11.13;
   // int radarPosY = (int)pHud->GetScreenSize(false)/24.8;

}
//
void CHUD::ToggleEnterPassengerButton(bool toggle)
{
    isEnterPassengerButtOn = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID ToggleEnterPassengerButton = env->GetMethodID(clazz, "ToggleEnterPassengerButton", "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleEnterPassengerButton, toggle);
}

void CHUD::ToggleEnterExitVehicleButton(bool toggle)
{
    isEnterExitVehicleButtonOn = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID ToggleEnterExitVehicleButton = env->GetMethodID(clazz, "ToggleEnterExitVehicleButton", "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleEnterExitVehicleButton, toggle);
}

void CHUD::ToggleLockVehicleButton(bool toggle)
{
    isLockVehicleButtonOn = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(jHudManager);

    jmethodID ToggleLockVehicleButton = env->GetMethodID(clazz, "ToggleLockVehicleButton", "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleLockVehicleButton, toggle);
}

void CHUD::ToggleHornButton(bool toggle)
{
    isHornButtonOn = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(jHudManager);

    jmethodID ToggleHornButton = env->GetMethodID(clazz, "ToggleHornButton", "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleHornButton, toggle);
}

int CHUD::GetScreenSize(bool isWidth)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return 0;
    }
    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID jGetScreenSize = env->GetMethodID(clazz, "GetScreenSize", "(Z)I");
    jint value = env->CallIntMethod(jHudManager, jGetScreenSize, isWidth);
    return value;
}
int tickUpdate;
void CHUD::UpdateHudInfo()
{
    tickUpdate++;
    if(tickUpdate < 20) return;

    tickUpdate = 0;
    CPlayerPed* pPed = pGame->FindPlayerPed();

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    env->CallVoidMethod(jHudManager, jUpdateHudInfo,
                        (int)pPed->GetHealth(),
                        (int)pPed->GetArmour(),
                        (int)pGUI->GetEat(),
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwType,
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwAmmo,
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwAmmoInClip
                        );
}
void CHUD::UpdateWanted()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID UpdateWanted = env->GetMethodID(clazz, "UpdateWanted", "(I)V");

    env->CallVoidMethod(jHudManager, UpdateWanted, pGame->GetWantedLevel());
}

void CHUD::UpdateMoney()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID UpdateMoney = env->GetMethodID(clazz, "UpdateMoney", "(I)V");

    env->CallVoidMethod(jHudManager, UpdateMoney, localMoney);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_ClickEnterPassengerButton(JNIEnv *env, jobject thiz) {
    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(pPlayer != nullptr) {
        pPlayer->GoEnterVehicle(true);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_ClickEnterExitVehicleButton(JNIEnv *env, jobject thiz) {
    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(pPlayer != nullptr && pPlayer->GetPlayerPed()->IsInVehicle()) {
        pPlayer->GetPlayerPed()->ExitCurrentVehicle();

        CVehiclePool *pVehiclePool=pNetGame->GetVehiclePool();
        VEHICLEID VehicleID=pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)GamePool_FindPlayerPed()->pVehicle);

        pPlayer->SendExitVehicleNotification(VehicleID);
    }
    else {
        pPlayer->GoEnterVehicle(false);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_ClickLockVehicleButton(JNIEnv *env, jobject thiz) {
    pNetGame->SendChatCommand("/lock");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_PressedHorn(JNIEnv *env, jobject thiz, jboolean pressed) {
    pGame->isHornActive = pressed;
}
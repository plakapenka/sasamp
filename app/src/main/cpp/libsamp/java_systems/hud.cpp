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
extern CNetGame *pNetGame;
extern CHUD *pHud;
extern CGUI* pGUI;

jobject jHudManager;

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
}

void CHUD::ToggleAll(bool toggle, bool withchat, bool anyway)
{
    if(!toggle)
    {
        hudhideCount ++;
    }
    else
    {
        hudhideCount--;
        if(hudhideCount > 0 && !anyway)return;
        else hudhideCount = 0;
    }
    isHudToggle = toggle;
    pGame->DisplayHUD(toggle);

    if(withchat)pGame->ToggleHUDElement(HUD_ELEMENT_CHAT, toggle);
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
}

void CHUD::UpdateHudInfo()
{
    PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID jUpdateHudInfo = env->GetMethodID(clazz, "UpdateHudInfo", "(IIIIIIII)V");

    env->CallVoidMethod(jHudManager, jUpdateHudInfo,
                        (int)pGame->FindPlayerPed()->GetHealth(),
                        (int)pGame->FindPlayerPed()->GetArmour(),
                        (int)pGUI->GetEat(),
                        (int)pPedPlayer->WeaponSlots[pPedPlayer->byteCurWeaponSlot].dwType,
                        (int)pPedPlayer->WeaponSlots[pPedPlayer->byteCurWeaponSlot].dwAmmo,
                        (int)pPedPlayer->WeaponSlots[pPedPlayer->byteCurWeaponSlot].dwAmmoInClip,
                        pGame->GetLocalMoney(),
                        pGame->GetWantedLevel()
                        );
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_cppToggleAllHud(JNIEnv *env, jobject thiz, jboolean toggle) {
    pHud->ToggleAll(toggle);
}

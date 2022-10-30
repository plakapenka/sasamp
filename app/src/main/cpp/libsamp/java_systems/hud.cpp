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
    jUpdateHudInfo = env->GetMethodID(env->GetObjectClass(thiz), "UpdateHudInfo", "(IIIIIIII)V");
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

  //  int radarPosX = (int)pHud->GetScreenSize(true)/11.13;
   // int radarPosY = (int)pHud->GetScreenSize(false)/24.8;

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

void CHUD::UpdateHudInfo()
{

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
                        (int)pPed->m_pPed->WeaponSlots[pPed->m_pPed->byteCurWeaponSlot].dwAmmoInClip,
                        localMoney,
                        pGame->GetWantedLevel()
                        );
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_cppToggleAllHud(JNIEnv *env, jobject thiz, jboolean toggle) {
    pHud->ToggleAll(toggle);
}

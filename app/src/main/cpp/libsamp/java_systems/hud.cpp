//
// Created by plaka on 28.10.2022.
//

#include "hud.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"
#include "util/armhook.h"

extern CSettings* pSettings;
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

void CHUD::ChangeChatHeight(int height) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID ChangeChatHeight = env->GetMethodID(clazz, "ChangeChatHeight", "(I)V");

    env->CallVoidMethod(jHudManager, ChangeChatHeight, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_HudInit(JNIEnv *env, jobject thiz) {

    jHudManager = env->NewGlobalRef(thiz);
    jUpdateHudInfo = env->GetMethodID(env->GetObjectClass(thiz), "UpdateHudInfo", "(IIIIII)V");

    pHud->ToggleHpText(pSettings->GetWrite().iHPArmourText);
    pHud->ChangeChatHeight(pSettings->GetWrite().iChatMaxMessages);

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

    pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, toggle);

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

    *(uint8_t*)(g_libGTASA+0x7165E8) = 0;
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

        CVehiclePool *pVehiclePool=pNetGame->GetVehiclePool();
        VEHICLEID VehicleID=pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)GamePool_FindPlayerPed()->pVehicle);

        if(!VehicleID)return;

//        VEHICLE_TYPE* CheckVeh = GamePool_Vehicle_GetAt(VehicleID);
//
//        if (CheckVeh->fHealth == 0.0f) return;
//        if (CheckVeh->entity.vtable == g_libGTASA + 0x5C7358) return;

        pPlayer->GetPlayerPed()->ExitCurrentVehicle();

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
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Speedometer_ClickSpedometr(JNIEnv *env, jobject thiz, jint turn_id,
                                                      jboolean toggle) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_TURN_SIGNAL;
    uint8_t button = turn_id;
    uint8_t toggle_ = toggle;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    bsSend.Write(toggle_);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void CNetGame::Packet_MAFIA_WAR(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint16_t time, attack_score, def_score;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(time);
    bs.Read(attack_score);
    bs.Read(def_score);

    pHud->UpdateOpgWarLayout(time, attack_score, def_score);
}
extern CKeyBoard *pKeyBoard;

void CHUD::UpdateOpgWarLayout(int time, int attack_score, int def_score)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID UpdateOpgWarLayout = env->GetMethodID(clazz, "UpdateOpgWarLayout", "(III)V");

    if(pKeyBoard->IsOpen())
    {
        env->CallVoidMethod(jHudManager, UpdateOpgWarLayout, 0, 0, 0);
        return;
    }

    env->CallVoidMethod(jHudManager, UpdateOpgWarLayout, time, attack_score, def_score);

    if(!time){
        isMafia_war_layout_active = false;
    }
    else isMafia_war_layout_active = true;
}

void CHUD::AddToChatInput(const char ch[])
{
    char msg_utf[255];
    cp1251_to_utf8(msg_utf, ch);
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jstring jch = env->NewStringUTF(msg_utf);

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID AddToChatInput = env->GetMethodID(clazz, "AddToChatInput", "(Ljava/lang/String;)V");

    env->CallVoidMethod(jHudManager, AddToChatInput, jch);
}

void CHUD::ToggleChatInput(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();


    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID ToggleChatInput = env->GetMethodID(clazz, "ToggleChatInput", "(Z)V");

    env->CallVoidMethod(jHudManager, ToggleChatInput, toggle);
}

void CHUD::AddChatMessage(const char msg[])
{
    if(!jHudManager)return;

    char msg_utf[1024];
    cp1251_to_utf8(msg_utf, msg);
    //CChatWindow::FilterInvalidChars(msg_utf);

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    //
    jstring jmsg = env->NewStringUTF( ConvertColorToHtml(msg_utf) );

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID AddChatMessage = env->GetMethodID(clazz, "AddChatMessage", "(Ljava/lang/String;)V");

    env->CallVoidMethod(jHudManager, AddChatMessage, jmsg);
}

void CHUD::ToggleHpText(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID ToggleHpText = env->GetMethodID(clazz, "ToggleHpText", "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleHpText, toggle);
}

void CHUD::ToggleChat(bool toggle){
    isChatOn = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jHudManager);
    jmethodID ToggleChat = env->GetMethodID(clazz, "ToggleChat" , "(Z)V");
    env->CallVoidMethod(jHudManager, ToggleChat, toggle);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_SetRadarBgPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1,
                                                    jfloat x2, jfloat y2) {
    pHud->radarbgx1 = x1;
    pHud->radarbgy1 = y1;

    pHud->radarbgx2 = x2;
    pHud->radarbgy2 = y2;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_SetRadarPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1) {

    pHud->radarx1 = x1;
    pHud->radary1 = y1;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_SendChatMessage(JNIEnv *env, jobject thiz, jbyteArray str) {
    jbyte* pMsg = env->GetByteArrayElements(str, nullptr);
    jsize length = env->GetArrayLength(str);

    std::string szStr((char*)pMsg, length);
    //const char *inputText = pEnv->GetStringUTFChars(str, nullptr);

    if(pNetGame) {
        pNetGame->SendChatMessage(const_cast<char *>(szStr.c_str()));

    }

    env->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_ToggleKeyBoard(JNIEnv *env, jobject thiz, jboolean toggle) {
    if (pKeyBoard) {
        if (toggle) {
            pKeyBoard->Open();
        } else {
            pKeyBoard->Close();
        }
    }
}
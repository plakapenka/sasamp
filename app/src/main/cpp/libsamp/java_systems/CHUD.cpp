//
// Created by plaka on 28.10.2022.
//

#include "CHUD.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"

extern CSettings* pSettings;
extern CJavaWrapper *g_pJavaWrapper;
extern CGame *pGame;
extern CGUI* pGUI;

bool        CHUD::bIsShow = false;
bool        CHUD::bIsShowPassengerButt = false;
bool        CHUD::bIsShowEnterExitButt = false;
bool        CHUD::bIsShowLockButt = false;
bool        CHUD::bIsShowHornButt = false;
bool        CHUD::bIsShowChat = true;
int         CHUD::iLocalMoney = 0;
int         CHUD::iWantedLevel = 0;
bool        CHUD::bIsShowMafiaWar = false;
float       CHUD::fLastGiveDamage = 0.0f;
PLAYERID    CHUD::lastGiveDamagePlayerId = INVALID_PLAYER_ID;

CVector2DFloat CHUD::radarBgPos1;
CVector2DFloat CHUD::radarBgPos2;
CVector2DFloat CHUD::radarPos;

jobject CHUD::thiz = nullptr;

jmethodID jUpdateHudInfo;

void CHUD::ChangeChatHeight(int height) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "ChangeChatHeight", "(I)V");

    env->CallVoidMethod(thiz, method, height);
}

void CHUD::ChangeChatTextSize(int size) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "ChangeChatFontSize", "(I)V");

    env->CallVoidMethod(thiz, method, size);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_HudInit(JNIEnv *env, jobject thiz) {

    CHUD::thiz = env->NewGlobalRef(thiz);
    jUpdateHudInfo = env->GetMethodID(env->GetObjectClass(thiz), "UpdateHudInfo", "(IIIIII)V");

    CHUD::ToggleHpText(pSettings->GetWrite().iHPArmourText);
    CHUD::ChangeChatHeight(pSettings->GetWrite().iChatMaxMessages);
    CHUD::ChangeChatTextSize(pSettings->GetWrite().iChatFontSize);

}
extern bool showSpeedometr;
void CHUD::toggleAll(bool toggle, bool withchat)
{
    if(toggle == bIsShow)
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

    bIsShow = toggle;

    pGame->ToggleHUDElement(HUD_ELEMENT_BUTTONS, toggle);

   // pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(toggle, true);
    pGame->ToggleHUDElement(HUD_ELEMENT_FPS, toggle);

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);

    jmethodID method = env->GetMethodID(clazz, "toggleAll", "(Z)V");
    env->CallVoidMethod(thiz, method, toggle);

    *(uint8_t*)(g_libGTASA+0x7165E8) = 0;//дефолт худ офф
    ScriptCommand(&toggle_radar_blank, (int)toggle); // радар офф

}

void CHUD::togglePassengerButton(bool toggle)
{
    bIsShowPassengerButt = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(thiz);

    jmethodID method = env->GetMethodID(clazz, "togglePassengerButton", "(Z)V");
    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::toggleEnterExitButton(bool toggle)
{
    bIsShowEnterExitButt = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID ToggleEnterExitVehicleButton = env->GetMethodID(clazz, "toggleEnterExitButton", "(Z)V");
    env->CallVoidMethod(thiz, ToggleEnterExitVehicleButton, toggle);
}

void CHUD::toggleLockButton(bool toggle)
{
    bIsShowLockButt = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(thiz);

    jmethodID ToggleLockVehicleButton = env->GetMethodID(clazz, "toggleLockButton", "(Z)V");
    env->CallVoidMethod(thiz, ToggleLockVehicleButton, toggle);
}

void CHUD::toggleHornButton(bool toggle)
{
    bIsShowHornButt = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    jclass clazz = env->GetObjectClass(thiz);

    jmethodID ToggleHornButton = env->GetMethodID(clazz, "toggleHornButton", "(Z)V");
    env->CallVoidMethod(thiz, ToggleHornButton, toggle);
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

    env->CallVoidMethod(thiz, jUpdateHudInfo,
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
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "UpdateWanted", "(I)V");

    env->CallVoidMethod(thiz, method, iWantedLevel);
}

void CHUD::UpdateMoney()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "UpdateMoney", "(I)V");

    env->CallVoidMethod(thiz, method, iLocalMoney);
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
    if(!pNetGame)return;
    if(!pNetGame->GetPlayerPool())return;

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(pPed->IsInVehicle())
    {
        VEHICLEID vehicleId = pPed->GetCurrentSampVehicleID();

        if(vehicleId == INVALID_VEHICLE_ID) return;

        pPlayer->GetPlayerPed()->ExitCurrentVehicle();

        pPlayer->SendExitVehicleNotification(vehicleId);
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

void CNetGame::packetSalary(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint32_t salary;
    uint32_t lvl;
    float exp;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(salary);
    bs.Read(lvl);
    bs.Read(exp);


    CHUD::UpdateSalary(salary, lvl, exp);
}

void CNetGame::packetMafiaWar(Packet* p)
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

    CHUD::updateOpgWarLayout(time, attack_score, def_score);
}
extern CKeyBoard *pKeyBoard;

void CHUD::updateOpgWarLayout(int time, int attack_score, int def_score)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID UpdateOpgWarLayout = env->GetMethodID(clazz, "updateOpgWarLayout", "(III)V");

    if(pKeyBoard->IsOpen())
    {
        env->CallVoidMethod(thiz, UpdateOpgWarLayout, 0, 0, 0);
        return;
    }

    env->CallVoidMethod(thiz, UpdateOpgWarLayout, time, attack_score, def_score);

    if(!time){
        bIsShowMafiaWar = false;
    }
    else bIsShowMafiaWar = true;
}

void CHUD::UpdateSalary(int salary, int lvl, float exp)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID UpdateSalary = env->GetMethodID(clazz, "UpdateSalary", "(IIF)V");

    env->CallVoidMethod(thiz, UpdateSalary, salary, lvl, exp);
}

void CHUD::SetChatInput(const char ch[])
{
    char msg_utf[255];
    cp1251_to_utf8(msg_utf, ch);
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jstring jch = env->NewStringUTF(msg_utf);

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID AddToChatInput = env->GetMethodID(clazz, "AddToChatInput", "(Ljava/lang/String;)V");

    env->CallVoidMethod(thiz, AddToChatInput, jch);
}

void CHUD::ToggleChatInput(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();


    jclass clazz = env->GetObjectClass(thiz);
    jmethodID ToggleChatInput = env->GetMethodID(clazz, "ToggleChatInput", "(Z)V");

    env->CallVoidMethod(thiz, ToggleChatInput, toggle);
}

void CHUD::AddChatMessage(const char msg[])
{
    if(!thiz)return;

    char msg_utf[1024];
    cp1251_to_utf8(msg_utf, msg);
    //CChatWindow::FilterInvalidChars(msg_utf);

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    //
    jstring jmsg = env->NewStringUTF( ConvertColorToHtml(msg_utf) );

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID AddChatMessage = env->GetMethodID(clazz, "AddChatMessage", "(Ljava/lang/String;)V");

    env->CallVoidMethod(thiz, AddChatMessage, jmsg);
}

void CHUD::addGiveDamageNotify(PLAYERID Id, int weaponId, float damage)
{
    if(!pSettings->GetWrite().iIsEnableDamageInformer) return;

    CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool)return;
    if(!pPlayerPool->GetSlotState(Id))return;

    if(damage > 200) damage = 200.0f;

    if(lastGiveDamagePlayerId == Id) {
        fLastGiveDamage += damage;
    }
    else {
        lastGiveDamagePlayerId = Id;
        fLastGiveDamage = damage;
    }

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jstring jnick = env->NewStringUTF( pPlayerPool->GetPlayerName(Id) );
    jstring jweap = env->NewStringUTF( CUtil::GetWeaponName(weaponId) );

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "addGiveDamageNotify", "(Ljava/lang/String;Ljava/lang/String;F)V");

    env->CallVoidMethod(thiz, method, jnick, jweap, fLastGiveDamage);
}

void CHUD::addTakeDamageNotify(char nick[], int weaponId, float damage)
{
    if(!pSettings->GetWrite().iIsEnableDamageInformer) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(damage > 200) damage = 200.0f;
    jstring jnick = env->NewStringUTF( nick );
    jstring jweap = env->NewStringUTF( CUtil::GetWeaponName(weaponId) );

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "addTakeDamageNotify", "(Ljava/lang/String;Ljava/lang/String;F)V");

    env->CallVoidMethod(thiz, method, jnick, jweap, damage);
}

void CHUD::ToggleHpText(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "ToggleHpText", "(Z)V");
    env->CallVoidMethod(thiz, method, toggle);
}

void CHUD::ToggleChat(bool toggle){
    bIsShowChat = toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID ToggleChat = env->GetMethodID(clazz, "ToggleChat" , "(Z)V");
    env->CallVoidMethod(thiz, ToggleChat, toggle);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_SetRadarBgPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1,
                                                    jfloat x2, jfloat y2) {
    CHUD::radarBgPos1.X = x1;
    CHUD::radarBgPos1.Y = y1;

    CHUD::radarBgPos2.X = x2;
    CHUD::radarBgPos2.Y = y2;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_SetRadarPos(JNIEnv *env, jobject thiz, jfloat x1, jfloat y1) {

    CHUD::radarPos.X = x1;
    CHUD::radarPos.Y = y1;

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
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_HudManager_clickCameraMode(JNIEnv *env, jobject thiz) {
    CFirstPersonCamera::Toggle();
}
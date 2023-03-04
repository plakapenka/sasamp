//
// Created by plaka on 14.01.2023.
//

#include "CMedic.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CMedic::thiz = nullptr;
bool CMedic::bIsShow = false;

void CMedic::showPreDeath(char* nick, int id)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jstring jNick = env->NewStringUTF(nick);

    jclass clazz = env->GetObjectClass(CMedic::thiz);
    jmethodID method = env->GetMethodID(clazz, "showPreDeath", "(Ljava/lang/String;I)V");

    env->CallVoidMethod(CMedic::thiz, method, jNick, id);
    env->DeleteLocalRef(jNick);

    CMedic::bIsShow = true;
}

void CMedic::showMedGame(char* nick)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jstring jNick = env->NewStringUTF(nick);

    jclass clazz = env->GetObjectClass(CMedic::thiz);
    jmethodID method = env->GetMethodID(clazz, "showMiniGame", "(Ljava/lang/String;)V");

    env->CallVoidMethod(CMedic::thiz, method, jNick);
    env->DeleteLocalRef(jNick);

    CMedic::bIsShow = true;
}

void CMedic::hide()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(CMedic::thiz);
    jmethodID method = env->GetMethodID(clazz, "hide", "()V");

    env->CallVoidMethod(CMedic::thiz, method);

    CMedic::bIsShow = false;
}

void CNetGame::packetPreDeath(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t toggle;
    uint16_t killerId;
    char* killername;

    bs.Read(toggle);
    bs.Read(killerId);

    if (toggle == 1) {
        CPlayerPool *pPlayerPool = GetPlayerPool();
        if (pPlayerPool) {
            killername =  pPlayerPool->GetPlayerName(killerId);

            CMedic::showPreDeath(killername, killerId);
        }
    }
}

void CNetGame::packetMedGame(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t strLen;
    char nick[MAX_PLAYER_NAME];

    bs.Read(strLen);
    bs.Read(nick, strLen);
    nick[strLen] = '\0';

    char utf8_nick[MAX_PLAYER_NAME];
    cp1251_to_utf8(utf8_nick, nick);

    CMedic::showMedGame(utf8_nick);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_PreDeath_init(JNIEnv *env, jobject thiz) {
    CMedic::thiz = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_PreDeath_medicMiniGameExit(JNIEnv *env, jobject thiz, jint type_id) {
    CMedic::bIsShow = false;

    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_MED_GAME;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write((uint8_t)type_id);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_PreDeath_medicPreDeathExit(JNIEnv *env, jobject thiz, jint button_id) {
    CMedic::bIsShow = false;

    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_PRE_DEATH;
    uint8_t button = button_id;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
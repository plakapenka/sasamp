//
// Created by plaka on 14.01.2023.
//

#include "CMedic.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "util/CJavaWrapper.h"

jobject CMedic::thiz = nullptr;
bool CMedic::bIsShow = false;

extern CJavaWrapper *g_pJavaWrapper;

void CMedic::showPreDeath(char* nick, int id)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jstring jNick = env->NewStringUTF(nick);

    jclass clazz = env->GetObjectClass(CMedic::thiz);
    jmethodID method = env->GetMethodID(clazz, "showPreDeath", "(Ljava/lang/String;I)V");

    env->CallVoidMethod(CMedic::thiz, method, jNick, id);

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

    CMedic::bIsShow = true;
}

//void CMedic::hidePreDeath()
//{
//    JNIEnv* env = g_pJavaWrapper->GetEnv();
//    if(!env)return;
//
//    jclass clazz = env->GetObjectClass(CMedic::thiz);
//    jmethodID method = env->GetMethodID(clazz, "showPreDeath", "(Ljava/lang/String;I)V");
//
//    env->CallVoidMethod(CMedic::thiz, method, jNick, id);
//}

void CNetGame::packetPreDeath(Packet* p) {
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t toggle;
    uint16_t killerId;
    char killername[25];

    bs.Read(toggle);
    bs.Read(killerId);

    if (toggle == 1) {
        CPlayerPool *pPlayerPool = GetPlayerPool();
        if (pPlayerPool) {
            if (pPlayerPool->GetSlotState(killerId)) {
                strcpy(killername, pPlayerPool->GetPlayerName(killerId));

            } else {
                strcpy(killername, "None");
            }

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

    cp1251_to_utf8(nick, nick);

    CMedic::showMedGame(nick);
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
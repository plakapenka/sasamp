//
// Created by plaka on 21.01.2023.
//

#include "CDuelsGui.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CDuelsGui::thiz = nullptr;

void CDuelsGui::clearKillList()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "clearKillList", "()V");

    env->CallVoidMethod(CDuelsGui::thiz, method);
}

void CDuelsGui::addMessage(PLAYERID killer, PLAYERID killee, int reason, int team)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;
    if(!pNetGame)return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool)return;

    jstring jKillername = env->NewStringUTF( pPlayerPool->GetPlayerName(killer) );
    jstring jKilleename = env->NewStringUTF( pPlayerPool->GetPlayerName(killee) );

    jclass clazz = env->GetObjectClass(CDuelsGui::thiz);
    jmethodID method = env->GetMethodID(clazz, "addItem", "(Ljava/lang/String;Ljava/lang/String;II)V");

    env->CallVoidMethod(CDuelsGui::thiz, method, jKillername, jKilleename, reason, team);
}

void CNetGame::packetKillList(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t killer;
    uint16_t killee;
    uint16_t reason;
    uint8_t team;

    bs.Read(killer);
    bs.Read(killee);
    bs.Read(reason);
    bs.Read(team);

    CDuelsGui::addMessage(killer, killee, reason, team);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Killist_init(JNIEnv *env, jobject thiz) {
    CDuelsGui::thiz = env->NewGlobalRef(thiz);
}
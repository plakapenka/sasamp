//
// Created by plaka on 06.12.2022.
//

#include "CCasino.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

#include "CHUD.h"

extern CJavaWrapper *g_pJavaWrapper;
extern CHUD *pHud;

jobject jCasino;
bool CCasino::isCasinoChipToggle = false;

void CNetGame::Packet_CasinoChip(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint8_t type;
    uint32_t balance;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(type);
    bs.Read(balance);

    if(!type) {
        balance = pHud->localMoney;
    }
    CCasino::OpenChipBuySell(!type, balance);
}

void CCasino::OpenChipBuySell(bool isSell, int balance){
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jCasino);
    jmethodID OpenChipBuySell = env->GetMethodID(clazz, "OpenChipBuySell", "(ZI)V");

    env->CallVoidMethod(jCasino, OpenChipBuySell, isSell, balance);

    CCasino::isCasinoChipToggle = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Casino_Init(JNIEnv *env, jobject thiz) {
    jCasino = env->NewGlobalRef(thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Casino_ClickChipButton(JNIEnv *env, jobject thiz, jint button_id,
                                                  jlong input, jboolean sell_or_buy) {
    CCasino::isCasinoChipToggle = false;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_CASINO_BUY_CHIP);
    bsSend.Write((uint8_t)sell_or_buy);
    bsSend.Write((uint8_t)button_id);
    bsSend.Write((uint32_t)input);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
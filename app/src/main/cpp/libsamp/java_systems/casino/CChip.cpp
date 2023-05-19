//
// Created by plaka on 06.12.2022.
//

#include "CChip.h"
#include <jni.h>

#include "main.h"

#include "game/game.h"
#include "net/netgame.h"

#include "java_systems/CHUD.h"


jobject jCasino;
bool CChip::bIsShow = false;

void CNetGame::packetCasinoChip(Packet* p)
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
        balance = CHUD::iLocalMoney;
    }
    CChip::OpenChipBuySell(!type, balance);
}

void CChip::OpenChipBuySell(bool isSell, int balance){
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jCasino);
    jmethodID OpenChipBuySell = env->GetMethodID(clazz, "OpenChipBuySell", "(ZI)V");

    env->CallVoidMethod(jCasino, OpenChipBuySell, isSell, balance);

    CChip::bIsShow = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_Casino_Init(JNIEnv *env, jobject thiz) {
    jCasino = env->NewGlobalRef(thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_Casino_ClickChipButton(JNIEnv *env, jobject thiz, jint button_id,
                                                  jlong input, jboolean sell_or_buy) {
    CChip::bIsShow = false;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_CASINO_BUY_CHIP);
    bsSend.Write((uint8_t)sell_or_buy);
    bsSend.Write((uint8_t)button_id);
    bsSend.Write((uint32_t)input);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

//
// Created by plaka on 19.01.2023.
//

#include "CBaccarat.h"

#include <jni.h>

#include "main.h"

#include "game/game.h"
#include "gui/gui.h"
#include "../../net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CBaccarat::thiz = nullptr;
jclass  CBaccarat::clazz = nullptr;
extern CJavaWrapper *g_pJavaWrapper;

enum PACKET_BACCARAT_TYPE
{
    PACKET_ADD_BET,
    PACKET_UPDATE,
};

void CBaccarat::updateBaccarat(int redCard, int yellowCard, int totalPlayers, int totalRed, int totalYellow, int totalGreen, int time, int betType, int betSum) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "update", "(IIIIIIIII)V");

    env->CallVoidMethod(CBaccarat::thiz, method, redCard, yellowCard, totalPlayers, totalRed, totalYellow, totalGreen, time, betType, betSum);
}

void CNetGame::packetCasinoBaccarat(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t packetType;
    bs.Read(packetType);

    switch (packetType) {
        case PACKET_ADD_BET: {
            uint8_t betType;
            uint32_t betSum;

            bs.Read(betType);
            bs.Read(betSum);
            break;
        }
        case PACKET_UPDATE: {
            uint8_t redCard;
            uint8_t yellowCard;
            uint8_t totalPlayers;
            uint8_t totalRed;
            uint8_t totalYellow;
            uint8_t totalGreen;
            int8_t time;

            uint8_t betType;
            uint32_t betSum;

            bs.Read(redCard);
            bs.Read(yellowCard);
            bs.Read(totalPlayers);
            bs.Read(totalRed);
            bs.Read(totalYellow);
            bs.Read(totalGreen);
            bs.Read(time);
            bs.Read(betType);
            bs.Read(betSum);

            CBaccarat::updateBaccarat(redCard, yellowCard, totalPlayers, totalRed, totalYellow, totalGreen, time, betType, betSum);
            break;
        }
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_CasinoBaccarat_sendAddBet(JNIEnv *env, jobject thiz, jint sum,
                                                     jint bettype) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_UPDATE_BACCARAT);

    bsSend.Write((uint8_t)PACKET_ADD_BET);

    bsSend.Write((uint32_t)sum);
    bsSend.Write((uint8_t)bettype);

    Log("Send %d, %d", sum, bettype);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_CasinoBaccarat_init(JNIEnv *env, jobject thiz) {
    CBaccarat::thiz = env->NewGlobalRef(thiz);
    CBaccarat::clazz = env->GetObjectClass(CBaccarat::thiz);
}
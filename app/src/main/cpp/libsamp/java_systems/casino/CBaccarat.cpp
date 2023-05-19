//
// Created by plaka on 19.01.2023.
//

#include "CBaccarat.h"

#include <jni.h>

#include "main.h"

#include "game/game.h"
#include "../../net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CBaccarat::thiz = nullptr;
jclass  CBaccarat::clazz = nullptr;

bool CBaccarat::bIsShow = false;
extern CJavaWrapper *g_pJavaWrapper;

enum PACKET_BACCARAT_TYPE
{
    PACKET_ADD_BET,
    PACKET_UPDATE,
    PACKET_EXIT,
    PACKET_UPDATE_LAST_WINS
};

void CBaccarat::updateLastWins(uint8_t* lastwins)
{
    Log("updateLastWins cpp");
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "updateLastWins", "([I)V");

    jint idArray[BACCARAT_MAX_HISTORY];
    for (int i = 0; i < BACCARAT_MAX_HISTORY ; i++) {
        idArray[i] = lastwins[i];
    }

    jintArray array = env->NewIntArray(BACCARAT_MAX_HISTORY);
    env->SetIntArrayRegion(array, 0, BACCARAT_MAX_HISTORY, idArray);
    //array[0] = lastwins[0];

    env->CallVoidMethod(CBaccarat::thiz, method, array);
}

void CBaccarat::updateBaccarat(int redCard, int yellowCard, int totalPlayers, int totalRed, int totalYellow, int totalGreen, int time, int betType, int betSum, int winner, int balance)
{
    CBaccarat::bIsShow = true;

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "update", "(IIIIIIIIIII)V");

    env->CallVoidMethod(CBaccarat::thiz, method, redCard, yellowCard, totalPlayers, totalRed, totalYellow, totalGreen, time, betType, betSum, winner, balance);
}

void CBaccarat::tempToggle(bool toggle) {

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "tempToggle", "(Z)V");

    env->CallVoidMethod(CBaccarat::thiz, method, toggle);
}

void CBaccarat::hide() {

    JNIEnv *env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID method = env->GetMethodID(clazz, "hide", "()V");

    env->CallVoidMethod(CBaccarat::thiz, method);
}

void CNetGame::packetCasinoBaccarat(Packet* p) {
    RakNet::BitStream bs((unsigned char *) p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t type_packet;
    bs.Read(type_packet);

    switch (type_packet) {
        case PACKET_BACCARAT_TYPE::PACKET_EXIT: {
            Log("PACKET_EXIT");
            CBaccarat::bIsShow = false;
            CBaccarat::hide();
            break;
        }
        case PACKET_BACCARAT_TYPE::PACKET_UPDATE_LAST_WINS: {
            uint8_t lastwins[CBaccarat::BACCARAT_MAX_HISTORY];

            for(int i = 0; i < CBaccarat::BACCARAT_MAX_HISTORY; i++)
            {
                bs.Read(lastwins[i]);
            }
            CBaccarat::updateLastWins(lastwins);
            break;
        }
        case PACKET_BACCARAT_TYPE::PACKET_UPDATE: {
            uint8_t redCard;
            uint8_t yellowCard;
            uint8_t totalPlayers;
            uint8_t totalRed;
            uint8_t totalYellow;
            uint8_t totalGreen;
            int8_t time;
            uint8_t winner;

            uint8_t betType;
            uint32_t betSum;
            uint32_t balance;

            bs.Read(redCard);
            bs.Read(yellowCard);
            bs.Read(totalPlayers);
            bs.Read(totalRed);
            bs.Read(totalYellow);
            bs.Read(totalGreen);
            bs.Read(time);
            bs.Read(winner);

            bs.Read(betType);
            bs.Read(betSum);
            bs.Read(balance);

            CBaccarat::updateBaccarat(redCard, yellowCard, totalPlayers, totalRed, totalYellow,
                                      totalGreen,
                                      time, betType, betSum, winner, balance);
            break;
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_CasinoBaccarat_sendAddBet(JNIEnv *env, jobject thiz, jint sum,
                                                     jint bettype) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_UPDATE_BACCARAT);

    bsSend.Write((uint8_t)PACKET_ADD_BET);

    bsSend.Write((int32_t)sum);
    bsSend.Write((uint8_t)bettype);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_CasinoBaccarat_init(JNIEnv *env, jobject thiz) {
    CBaccarat::thiz = env->NewGlobalRef(thiz);
    CBaccarat::clazz = env->GetObjectClass(CBaccarat::thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_CasinoBaccarat_exit(JNIEnv *env, jobject thiz) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_UPDATE_BACCARAT);

    bsSend.Write((uint8_t)PACKET_EXIT);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_CasinoBaccarat_close(JNIEnv *env, jobject thiz) {
    // TODO: implement close()
}
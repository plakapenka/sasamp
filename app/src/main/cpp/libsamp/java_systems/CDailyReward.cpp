//
// Created by plaka on 31.01.2023.
//

#include "CDailyReward.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CDailyReward::thiz = nullptr;
bool CDailyReward::bIsShow = false;

void CDailyReward::show(int day, int second)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(CDailyReward::thiz);
    jmethodID method = env->GetMethodID(clazz, "show", "(II)V");

    env->CallVoidMethod(CDailyReward::thiz, method, day, second);

    CDailyReward::bIsShow = true;
}

void CNetGame::packetDailyRewards(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t day;
    int16_t second;

    bs.Read(day);
    bs.Read(second);

    CDailyReward::show(day, second);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_DailyReward_init(JNIEnv *env, jobject thiz) {
    CDailyReward::thiz = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_DailyReward_click(JNIEnv *env, jobject thiz) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_DAILY_REWARDS;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_DailyReward_hide(JNIEnv *env, jobject thiz) {
    CDailyReward::bIsShow = false;
}
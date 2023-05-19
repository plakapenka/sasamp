//
// Created by plaka on 16.02.2023.
//

#include "CDonate.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jclass CDonate::clazz = nullptr;
jobject CDonate::thiz  = nullptr;

void CDonate::show(int balance)
{
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if(CDonate::thiz == nullptr) {

        jmethodID constructor = env->GetMethodID(CDonate::clazz, "<init>",
                                                 "(Landroid/app/Activity;I)V");
        CDonate::thiz = env->NewObject(CDonate::clazz, constructor, g_pJavaWrapper->activity,
                                       balance);
        CDonate::thiz = env->NewGlobalRef(CDonate::thiz);
    }

}

void CDonate::updateBalance(int balance)
{
    if(CDonate::thiz == nullptr)return;

    JNIEnv *env = g_pJavaWrapper->GetEnv();
    jmethodID method = env->GetMethodID(CDonate::clazz, "updateBalance", "(I)V");
    env->CallVoidMethod(CDonate::thiz, method, balance);
}

void CNetGame::packetShowDonat(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t type;
    bs.Read(type);

    uint32_t balance;
    bs.Read(balance);

    if(type == 4)
    {// show
        CDonate::show((int) balance);
    }
    else if (type == 5){
        CDonate::updateBalance(balance);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_donate_Donate_native_1hide(JNIEnv *env, jobject thiz) {
    env->DeleteGlobalRef(CDonate::thiz);
    CDonate::thiz = nullptr;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_donate_Donate_sendAction(JNIEnv *env, jobject thiz, jint action_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_DONATE;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write((uint8_t) action_id);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_donate_Donate_sendClickItem(JNIEnv *env, jobject thiz, jint action_type,
                                                       jint button_id, jint item_type,
                                                       jint item_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_DONATE;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write((uint8_t) action_type);
    bsSend.Write((uint8_t) button_id);
    bsSend.Write((uint16_t) item_type);
    bsSend.Write((uint16_t) item_id);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
//
// Created by plaka on 01.02.2023.
//

#include "CNotification.h"
#include <jni.h>
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CNotification::thiz = nullptr;

void CNotification::hide() {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }

    jclass clazz = env->GetObjectClass(CNotification::thiz);
    jmethodID method = env->GetMethodID(clazz, "hide", "(Z)V");

    env->CallVoidMethod(CNotification::thiz, method, true);
}

void CNotification::show(int type, char* text, int duration, int actionId) {
    JNIEnv *env = g_pJavaWrapper->GetEnv();

    if (!env) {
        Log("No env");
        return;
    }
    jstring jtext = env->NewStringUTF(text);

    jclass clazz = env->GetObjectClass(CNotification::thiz);
    jmethodID method = env->GetMethodID(clazz, "ShowNotification", "(ILjava/lang/String;II)V");

    env->CallVoidMethod(CNotification::thiz, method, type, jtext, duration, actionId);
}

void CNetGame::packetNotification(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint16_t type;
    char str[256];
    uint8_t len;
    uint8_t time;
    uint16_t actionId;
    bs.Read(type);
    bs.Read(len);
    bs.Read(&str[0], len);
    bs.Read(time);
    bs.Read(actionId);

    char utf_str[256];
    cp1251_to_utf8(utf_str, str, len);

    if (type != 65535)
    {
        CNotification::show(type, (char *) utf_str, time, actionId);
    }
    else
    {
        CNotification::hide();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Notification_sendClick(JNIEnv *env, jobject thiz, jint action_id,
                                                  jint button_id) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_ACTION_LABEL;
    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write( (uint16_t)action_id);
    bsSend.Write( (uint8_t)button_id);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Notification_init(JNIEnv *env, jobject thiz) {
    CNotification::thiz = env->NewGlobalRef(thiz);
}
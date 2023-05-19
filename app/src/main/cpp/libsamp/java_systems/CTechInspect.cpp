//
// Created by plaka on 05.02.2023.
//

#include "CTechInspect.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jobject CTechInspect::thiz = nullptr;
bool CTechInspect::bIsShow = false;

void CTechInspect::show(char name[], int gen, int candl, int brake, int starter, int nozzles, int price, int carid)
{
    CTechInspect::bIsShow = true;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jstring str = env->NewStringUTF(name);

    jclass clazz = env->GetObjectClass(CTechInspect::thiz);
    jmethodID method = env->GetMethodID(clazz, "show", "(Ljava/lang/String;IIIIIII)V");

    env->CallVoidMethod(CTechInspect::thiz, method, str, gen, candl, brake, starter, nozzles, price, carid);
    env->DeleteLocalRef(str);
}

void CNetGame::packetTechInspect(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    float candl;
    float brake;
    float starter;
    float gen;
    float fors;
    uint16_t price;
    uint16_t carid;

    uint16_t len;
    char name[255];

    bs.Read(candl);
    bs.Read(brake);
    bs.Read(starter);
    bs.Read(gen);
    bs.Read(fors);
    bs.Read(carid);
    bs.Read(price);
    bs.Read(len);
    bs.Read(name, len);
    name[len] = '\0';

    char utf_name[255];
    cp1251_to_utf8(utf_name, name);

    CTechInspect::show(utf_name, (int)gen, (int)candl, (int)brake, (int)starter, (int)fors, price, carid);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_TechIspect_init(JNIEnv *env, jobject thiz) {
    CTechInspect::thiz = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_TechIspect_sendClick(JNIEnv *env, jobject thiz)
{
    CTechInspect::bIsShow = false;

    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_TECH_INSPECT;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);

    pNetGame->GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_TechIspect_exit(JNIEnv *env, jobject thiz) {
    CTechInspect::bIsShow = false;
}
//
// Created by plaka on 10.12.2022.
//

#include "CAucContainer.h"

#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"

#include "CHUD.h"

jobject jAucContainer;
bool CAucContainer::bIsShow = false;

void CNetGame::packetAucContainer(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint16_t id;
    uint8_t type;
    uint32_t price;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(id);
    bs.Read(type);
    bs.Read(price);

    CAucContainer::show(id, type, (int)price);
}

void CAucContainer::show(int id, int type, int price){
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jAucContainer);
    jmethodID Show = env->GetMethodID(clazz, "show", "(III)V");

    env->CallVoidMethod(jAucContainer, Show, id, type, price);

    CAucContainer::bIsShow = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_AucContainer_Init(JNIEnv *env, jobject thiz) {
    jAucContainer = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_AucContainer_SendClick(JNIEnv *env, jobject thiz, jint button_id) {
    CAucContainer::bIsShow = false;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_CONTEINER_AUC);
    bsSend.Write((uint8_t)button_id);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
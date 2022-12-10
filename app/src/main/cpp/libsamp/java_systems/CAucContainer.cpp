//
// Created by plaka on 10.12.2022.
//

#include "CAucContainer.h"

#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

#include "CHUD.h"

jobject jAucContainer;
bool CAucContainer::isAucContainerShow = false;

void CNetGame::Packet_AucContainer(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint8_t id;
    uint8_t type;
    uint32_t price;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(id);
    bs.Read(type);
    bs.Read(price);

    CAucContainer::Show(id, type, price);
}

void CAucContainer::Show(int id, int type, int price){
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jAucContainer);
    jmethodID Show = env->GetMethodID(clazz, "Show", "(III)V");

    env->CallVoidMethod(jAucContainer, Show, id, type, price);

    CAucContainer::isAucContainerShow = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AucContainer_Init(JNIEnv *env, jobject thiz) {
    jAucContainer = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AucContainer_SendClick(JNIEnv *env, jobject thiz, jint button_id) {
    CAucContainer::isAucContainerShow = false;

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_SHOW_CONTEINER_AUC);
    bsSend.Write((uint8_t)button_id);

    pNetGame->GetRakClient()->Send(&bsSend, MEDIUM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
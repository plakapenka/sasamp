//
// Created by plaka on 07.11.2022.
//

#include "hud.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

//extern CNetGame* pNetGame;
extern CJavaWrapper *g_pJavaWrapper;

jobject jFurnitureFactory = nullptr;

extern "C"
{
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Furniture_1factory_InitFurnitureFactory(JNIEnv *env, jobject thiz) {
jFurnitureFactory = env->NewGlobalRef(thiz);
}

JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Furniture_1factory_SendSucces(JNIEnv *env, jobject thiz,
        jint button_id) {
uint8_t packet = ID_CUSTOM_RPC;
uint8_t RPC = RPC_SHOW_FACTORY_GAME;
uint8_t button = button_id;


RakNet::BitStream bsSend;
bsSend.Write(packet);
bsSend.Write(RPC);
bsSend.Write(button);
pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
}

void CNetGame::Packet_FurnitureFactory(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint8_t show;
    uint32_t furniture;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(show);
    bs.Read(furniture);

    if(show)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jclass clazz = env->GetObjectClass(jFurnitureFactory);
        jmethodID ShowFactory = env->GetMethodID(clazz, "ShowFactory", "(I)V");

        env->CallVoidMethod(jFurnitureFactory, ShowFactory, (int)furniture);
    }
}
#include "CHUD.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "CInventory.h"
#include "chatwindow.h"

jobject CInventory::thiz = nullptr;
bool CInventory::bIsToggle = false;

void CInventory::toggleShow(bool toggle, float satiety)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(CInventory::thiz);
    jmethodID ToggleShow = env->GetMethodID(clazz, "toggleShow", "(ZFF)V");

    CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if (!pPlayer || !pPlayer->GetPlayerPed()) return;

    env->CallVoidMethod(CInventory::thiz, ToggleShow, toggle, satiety, pPlayer->GetPlayerPed()->GetHealth());
    bIsToggle = toggle;
}

void CInventory::updateItem(int matrixindex, int pos, const char sprite[], const char caption[], bool active) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jstring jsprite = env->NewStringUTF( sprite );
    jstring jcaption = env->NewStringUTF( caption );

    jclass clazz = env->GetObjectClass(CInventory::thiz);
    jmethodID method = env->GetMethodID(clazz, "updateItem", "(IILjava/lang/String;Ljava/lang/String;Z)V");


    env->CallVoidMethod(CInventory::thiz, method, matrixindex, pos, jsprite, jcaption, active);
}

void CInventory::updateCarryng(int matrixindex, int brutto, int maxbrutto) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(CInventory::thiz);
    jmethodID method = env->GetMethodID(clazz, "updateCarryng", "(III)V");


    env->CallVoidMethod(CInventory::thiz, method, matrixindex, brutto, maxbrutto);
}

void CInventory::updateItem(int matrixindex, int pos, bool active) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(CInventory::thiz);
    jmethodID method = env->GetMethodID(clazz, "itemToggleActive", "(IIZ)V");


    env->CallVoidMethod(CInventory::thiz, method, matrixindex, pos, active);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_InventoryInit(JNIEnv *env, jobject thiz) {
    CInventory::thiz = env->NewGlobalRef(thiz);
}

void CNetGame::packetInventoryItemActive(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint8_t matrixindex;
    uint8_t pos;
    bool active;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(matrixindex);
    bs.Read(pos);
    bs.Read(active);

    CInventory::updateItem(matrixindex, pos, active);

}

void CNetGame::packetInventoryToggle(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    bool toggle;
    float satiety;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(toggle);
    bs.Read(satiety);

    CInventory::toggleShow(toggle, satiety);

}

void CNetGame::packetInventoryUpdateCarryng(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint16_t matrixindex;
    uint16_t brutto;
    uint16_t maxbrutto;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(matrixindex);
    bs.Read(brutto);
    bs.Read(maxbrutto);

    CInventory::updateCarryng(matrixindex, brutto, maxbrutto);
}

void CNetGame::packetInventoryUpdateItem(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    uint16_t matrixindex, pos;
    bool active;

    uint8_t spriteLen;
    char sprite[123];

    uint8_t captionLen;
    char caption[123];

    bs.Read(packetID);
    bs.Read(rpcID);

    bs.Read(matrixindex);
    bs.Read(pos);

    bs.Read(spriteLen);
    bs.Read(sprite, spriteLen);
    sprite[spriteLen] = '\0';

    bs.Read(captionLen);
    bs.Read(caption, captionLen);
    caption[captionLen] = '\0';

    bs.Read(active);

    char utf8_1[123];
    cp1251_to_utf8(utf8_1, sprite);

    char utf8_2[123];
    cp1251_to_utf8(utf8_2, caption);

    CInventory::updateItem(matrixindex, pos, utf8_1, utf8_2, active);

    //Log("packetInventoryToggle %d", toggle);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_switchToggle(JNIEnv *env, jobject thiz, jboolean toggle) {
    CInventory::bIsToggle = toggle;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_sendSelectItem(JNIEnv *env, jobject thiz, jint matrindex,
                                                    jint pos) {

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_MATRIX_CLICK);
    bsSend.Write((uint8_t)matrindex);
    bsSend.Write((uint16_t)pos);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_sendClickButton(JNIEnv *env, jobject thiz, jint buttonid) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_INVENTAR_BUTTONS);
    bsSend.Write((uint8_t)buttonid);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
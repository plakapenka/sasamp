#include "hud.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "inventrory.h"

extern CJavaWrapper *g_pJavaWrapper;
extern CINVENTORY *pInventory;

jobject jInventory;

void CINVENTORY::ToggleShow(bool toggle, float satiety)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jInventory);
    jmethodID ToggleShow = env->GetMethodID(clazz, "ToggleShow", "(ZFF)V");

    CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if (pPlayer) {
        if (pPlayer->GetPlayerPed()) {
            env->CallVoidMethod(jInventory, ToggleShow, toggle, satiety, pPlayer->GetPlayerPed()->GetHealth());
            isToggle = toggle;
        }
    }
}

void CINVENTORY::InventoryUpdateItem(int matrixindex, int pos, const char sprite[], const char caption[], bool active) {
    Log("m = %d, pos = %d, sprite = %s, capt = %s, active = %d",matrixindex, pos, sprite, caption, active );
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jstring jsprite = env->NewStringUTF( sprite );
    jstring jcaption = env->NewStringUTF( caption );

    jclass clazz = env->GetObjectClass(jInventory);
    jmethodID InventoryUpdateItem = env->GetMethodID(clazz, "InventoryUpdateItem", "(IILjava/lang/String;Ljava/lang/String;Z)V");


    env->CallVoidMethod(jInventory, InventoryUpdateItem, matrixindex, pos, jsprite, jcaption, active);
}

void CINVENTORY::UpdateCarryng(int matrixindex, int brutto, bool maxbrutto) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jInventory);
    jmethodID UpdateCarryng = env->GetMethodID(clazz, "UpdateCarryng", "(III)V");


    env->CallVoidMethod(jInventory, UpdateCarryng, matrixindex, brutto, maxbrutto);
}

void CINVENTORY::UpdateItem(int matrixindex, int pos, bool active) {
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jInventory);
    jmethodID InventoryItemActive = env->GetMethodID(clazz, "InventoryItemActive", "(IIZ)V");


    env->CallVoidMethod(jInventory, InventoryItemActive, matrixindex, pos, active);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_InventoryInit(JNIEnv *env, jobject thiz) {
    jInventory = env->NewGlobalRef(thiz);
}

void CNetGame::Packet_InventoryItemActive(Packet* p)
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

    pInventory->UpdateItem(matrixindex, pos, active);

}

void CNetGame::Packet_InventoryToggle(Packet* p)
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

    pInventory->ToggleShow(toggle, satiety);

    Log("Packet_InventoryToggle %d", toggle);
}

void CNetGame::Packet_InventoryUpdateCarryng(Packet* p)
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

    pInventory->UpdateCarryng(matrixindex, brutto, maxbrutto);

}

void CNetGame::Packet_InventoryUpdateItem(Packet* p)
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

    pInventory->InventoryUpdateItem(matrixindex, pos, utf8_1, utf8_2, active);

    //Log("Packet_InventoryToggle %d", toggle);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_SwitchToggle(JNIEnv *env, jobject thiz, jboolean toggle) {
    pInventory->isToggle = toggle;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_SendSelectItem(JNIEnv *env, jobject thiz, jint matrindex,
                                                    jint pos) {

    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_MATRIX_CLICK);
    bsSend.Write((uint8_t)matrindex);
    bsSend.Write((uint16_t)pos);

    Log("matrindex - %d, pos - %d", matrindex, pos);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_SendClickButton(JNIEnv *env, jobject thiz, jint buttonid) {
    RakNet::BitStream bsSend;
    bsSend.Write((uint8_t)ID_CUSTOM_RPC);
    bsSend.Write((uint8_t)RPC_INVENTAR_BUTTONS);
    bsSend.Write((uint8_t)buttonid);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
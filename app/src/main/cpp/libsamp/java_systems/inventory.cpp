#include "hud.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "inventrory.h"

extern CJavaWrapper *g_pJavaWrapper;
extern CINVENTORY *pInventory;

jobject jInventory;

void CINVENTORY::ToggleShow(bool toggle)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jInventory);
    jmethodID ToggleShow = env->GetMethodID(clazz, "ToggleShow", "(Z)V");

    env->CallVoidMethod(jInventory, ToggleShow, toggle);

    isToggle = toggle;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Inventory_InventoryInit(JNIEnv *env, jobject thiz) {
    jInventory = env->NewGlobalRef(thiz);
}

void CNetGame::Packet_InventoryToggle(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
    uint8_t packetID;
    uint32_t rpcID;
    bool toggle;

    bs.Read(packetID);
    bs.Read(rpcID);
    bs.Read(toggle);

    pInventory->ToggleShow(toggle);

    Log("Packet_InventoryToggle %d", toggle);
}


#include "hud.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

//extern CNetGame* pNetGame;
extern CJavaWrapper *g_pJavaWrapper;

jobject jMine1;
jobject jMine2;
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_MineGame1_Init(JNIEnv *env, jobject thiz) {
    jMine1 = env->NewGlobalRef(thiz);
}

void CJavaWrapper::ShowMiningGame1() {
    JNIEnv* env = GetEnv();

    jclass clazz = env->GetObjectClass(jMine1);
    jmethodID Show = env->GetMethodID(clazz, "Show", "()V");

    env->CallVoidMethod(jMine1, Show);
}

void CJavaWrapper::ShowMiningGame2() {
    JNIEnv* env = GetEnv();

    jclass clazz = env->GetObjectClass(jMine2);
    jmethodID Show = env->GetMethodID(clazz, "Show", "()V");

    env->CallVoidMethod(jMine2, Show);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_MineGame2_Init(JNIEnv *env, jobject thiz) {
    jMine2 = env->NewGlobalRef(thiz);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_MineGame1_ExitMineGame(JNIEnv *env, jclass clazz, jint exittype) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_MINING_GAME;
    uint8_t button = exittype;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
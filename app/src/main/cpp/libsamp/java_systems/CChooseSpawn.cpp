//
// Created by plaka on 20.02.2023.
//

#include "CChooseSpawn.h"

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"

jobject CChooseSpawn::thiz = nullptr;
jclass CChooseSpawn::clazz = nullptr;

void CChooseSpawn::show(int organization, int station, int exit, int garage, int house)
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if(thiz == nullptr)
    {
        jmethodID constructor = env->GetMethodID(clazz, "<init>", "(Landroid/app/Activity;)V");
        thiz = env->NewObject(clazz, constructor, g_pJavaWrapper->activity);
        thiz = env->NewGlobalRef(thiz);
    }

    jmethodID method = env->GetMethodID(clazz, "show", "(IIIII)V");

    env->CallVoidMethod(thiz, method, organization, station, exit, garage, house);
}

void CChooseSpawn::hide()
{
    if(thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(clazz, "hide", "()V");
        env->CallVoidMethod(thiz, method);

        env->DeleteGlobalRef(thiz);
        thiz = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_ChooseSpawn_clickEnter(JNIEnv *env, jobject thiz, jint spawn_id) {
    pNetGame->SendCustomPacket(253, 2, spawn_id);
    Log("SendChooseSpawn: SpawnId - %d", spawn_id);
}
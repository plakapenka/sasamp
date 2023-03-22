//
// Created by plaka on 26.02.2023.
//

#include "LoadingScreen.h"
#include "main.h"
#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jobject LoadingScreen::thiz = nullptr;
jclass LoadingScreen::clazz = nullptr;

void LoadingScreen::updatePercent(int percent)
{
    if(LoadingScreen::thiz == nullptr)
        LoadingScreen::linkedObj();

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(LoadingScreen::clazz, "updatePercent", "(I)V");
    env->CallVoidMethod(LoadingScreen::thiz, method, percent);
}

void LoadingScreen::hide()
{
    if(LoadingScreen::thiz == nullptr)
        LoadingScreen::linkedObj();

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(LoadingScreen::clazz, "hide", "()V");
    env->CallVoidMethod(LoadingScreen::thiz, method);

    env->DeleteGlobalRef(LoadingScreen::clazz);
    LoadingScreen::clazz = nullptr;
}

void LoadingScreen::linkedObj()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    // construct

    jmethodID constructor = env->GetMethodID(LoadingScreen::clazz, "<init>", "(Landroid/app/Activity;)V");
    LoadingScreen::thiz = env->NewObject(LoadingScreen::clazz, constructor, g_pJavaWrapper->activity);
    LoadingScreen::thiz = env->NewGlobalRef(LoadingScreen::thiz);
}

void LoadingScreen::gtaLoadingScreenTick()
{
    const float percent = *(float*)(g_libGTASA + 0x009920C0)*2;
    if (percent <= 0.0f) return;

 //   LoadingScreen::updatePercent(percent);
}


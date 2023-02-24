//
// Created by plaka on 20.02.2023.
//

#include "CAuthorization.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"


jobject CAuthorization::thiz = nullptr;
jclass CAuthorization::clazz = nullptr;
extern CJavaWrapper *g_pJavaWrapper;

void CAuthorization::show(char *nick, int id, bool ip_match, bool toggleAutoLogin, bool email_acvive){
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if(CAuthorization::thiz == nullptr)
    {
        jmethodID constructor = env->GetMethodID(CAuthorization::clazz, "<init>", "(Landroid/app/Activity;)V");
        CAuthorization::thiz = env->NewObject(CAuthorization::clazz, constructor, g_pJavaWrapper->activity);
        CAuthorization::thiz = env->NewGlobalRef(CAuthorization::thiz);
    }

    jstring jnick = env->NewStringUTF( nick );

    jmethodID method = env->GetMethodID(CAuthorization::clazz, "show", "(Ljava/lang/String;IZZZ)V");
    env->CallVoidMethod(CAuthorization::thiz, method, jnick, id, ip_match, toggleAutoLogin, email_acvive);

    env->DeleteLocalRef(jnick);
}

void CAuthorization::hide()
{
    if(CAuthorization::thiz != nullptr)
    {
        JNIEnv* env = g_pJavaWrapper->GetEnv();

        jmethodID method = env->GetMethodID(CAuthorization::clazz, "hide", "()V");
        env->CallVoidMethod(CAuthorization::thiz, method);

        env->DeleteGlobalRef(CAuthorization::thiz);
        CAuthorization::thiz = nullptr;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AuthorizationManager_toggleAutoLogin(JNIEnv *env, jobject thiz,
                                                                jboolean toggle) {
    CSettings::m_Settings.szAutoLogin = toggle;
    CSettings::save();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AuthorizationManager_clickRecoveryPass(JNIEnv *env, jobject thiz) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_RESTORE_PASS;

    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);

    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AuthorizationManager_loginClick(JNIEnv *env, jobject thiz,
                                                           jstring pass) {
    const char *inputPassword = env->GetStringUTFChars(pass, nullptr);

    if(pNetGame) {
        pNetGame->SendLoginPacket(inputPassword);
    }

    Log("onAuthPlayClick: inputPassword - %s", inputPassword);

    env->ReleaseStringUTFChars(pass, inputPassword);
}
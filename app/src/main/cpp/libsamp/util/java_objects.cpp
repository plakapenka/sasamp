//
// Created by plaka on 26.10.2022.
//

#include "CJavaWrapper.h"

#include <jni.h>

extern "C"
{

JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_CasinoDice_CasinoInit(JNIEnv *env, jobject thiz) {

    g_pJavaWrapper->jCasinoDice = env->NewGlobalRef(thiz);
}
}
//
// Created by plaka on 01.02.2023.
//

#include "CDialog.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"
#include "java_systems/casino/CBaccarat.h"

jobject CDialog::thiz = nullptr;
bool    CDialog::bIsShow = false;

void CDialog::show(int id, int style, char caption[], char info[], char button1[], char button2[])
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if (!env)
    {
        Log("No env");
        return;
    }

    jstring j_caption = env->NewStringUTF( caption );
    jstring j_info = env->NewStringUTF( info );
    jstring j_button1 = env->NewStringUTF( button1 );
    jstring j_button2 = env->NewStringUTF( button2 );


    jclass clazz = env->GetObjectClass( CDialog::thiz );

    jmethodID method = env->GetMethodID(clazz, "show", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    env->CallVoidMethod(CDialog::thiz, method, id, style, j_caption, j_info, j_button1, j_button2);

    env->DeleteLocalRef(j_caption);
    env->DeleteLocalRef(j_info);
    env->DeleteLocalRef(j_button1);
    env->DeleteLocalRef(j_button2);

    CDialog::bIsShow = true;

    if(CBaccarat::bIsShow) CBaccarat::tempToggle(false);
}

void CDialog::rpcShowPlayerDialog(RPCParameters *rpcParams)
{
    unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    uint16_t wDialogID = -1;
    uint8_t byteDialogStyle = 0;
    uint8_t len;
    char szBuff[4096+1];
    char title[64 * 3 + 1], info[4096+1], button1[20+1], button2[20+1];

    RakNet::BitStream bsData((unsigned char *)Data,(iBitLength/8)+1,false);

    bsData.Read(wDialogID);
    bsData.Read(byteDialogStyle);

    // title
    bsData.Read(len);
    bsData.Read(szBuff, len);
    szBuff[len] = '\0';
    cp1251_to_utf8(title, szBuff);

    // button1
    bsData.Read(len);
    bsData.Read(szBuff, len);
    szBuff[len] = '\0';
    cp1251_to_utf8(button1, szBuff);

    // button2
    bsData.Read(len);
    bsData.Read(szBuff, len);
    szBuff[len] = '\0';
    cp1251_to_utf8(button2, szBuff);

    // info
    stringCompressor->DecodeString(szBuff, 4096, &bsData);
    cp1251_to_utf8(info, szBuff);

    if(wDialogID < 0) return;

    CDialog::show(wDialogID, byteDialogStyle, title, info, button1, button2);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_dialogs_Dialog_init(JNIEnv *env, jobject thiz) {
    CDialog::thiz = env->NewGlobalRef(thiz);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_dialogs_Dialog_sendResponse(JNIEnv *env, jobject thiz, jint button, jint id, jint item, jbyteArray str)
{
    jbyte* pMsg = env->GetByteArrayElements(str, nullptr);
    jsize length = env->GetArrayLength(str);

    std::string szStr((char*)pMsg, length);

    uint8_t respLen = strlen(szStr.c_str());

    RakNet::BitStream bsSend;
    bsSend.Write((uint16_t)id);
    bsSend.Write((uint8_t)button);
    bsSend.Write((uint16_t)item);

    bsSend.Write((uint8_t)respLen);
    bsSend.Write(const_cast<char *>(szStr.c_str()), respLen);

    pNetGame->m_pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);

    env->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
    CDialog::bIsShow = false;

    if(CBaccarat::bIsShow) CBaccarat::tempToggle(true);
}
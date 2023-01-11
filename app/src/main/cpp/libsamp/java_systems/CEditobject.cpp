//
// Created by plaka on 26.11.2022.
//

#include "CEditobject.h"
#include <jni.h>
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "keyboard.h"
#include "CSettings.h"
#include "chatwindow.h"
#include "util/CJavaWrapper.h"

bool CEditobject::bIsToggle = false;
int CEditobject::iEditedSlot = INVALID_EDITED_SLOT;
jobject jObjEditor;
extern CJavaWrapper *g_pJavaWrapper;

void CEditobject::StartEditAttachedObject(int slot)
{
    if(!pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->IsValidAttach(slot)){
        CChatWindow::AddDebugMessage("Invalid attach slot %d", slot);
        return;
    }
    CEditobject::iEditedSlot = slot;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jclass clazz = env->GetObjectClass(jObjEditor);
    jmethodID Toggle = env->GetMethodID(clazz, "Toggle", "(Z)V");

    env->CallVoidMethod(jObjEditor, Toggle, true);

    bIsToggle = true;
}

CEditobject::CEditobject(){ }

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AttachEdit_Init(JNIEnv *env, jobject thiz) {
    jObjEditor = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AttachEdit_Exit(JNIEnv *env, jobject thiz) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CEditobject::iEditedSlot;

    CEditobject::bIsToggle = false;
    CEditobject::iEditedSlot = INVALID_EDITED_SLOT;
    CEditobject::SendOnEditAttach(
            0,
            slot,
            pPlayer->m_aAttachedObjects[slot].dwModelId,
            pPlayer->m_aAttachedObjects[slot].dwBone,
            pPlayer->m_aAttachedObjects[slot].vecOffset,
            pPlayer->m_aAttachedObjects[slot].vecRotation,
            pPlayer->m_aAttachedObjects[slot].vecScale
            );
}

void CEditobject::SendOnEditAttach(int response, int index, int modelid, int bone, VECTOR offset, VECTOR rot, VECTOR scale){
    RakNet::BitStream bsSend;
    bsSend.Write((uint32_t)response);
    bsSend.Write((uint32_t)index);
    bsSend.Write((uint32_t)modelid);
    bsSend.Write((uint32_t)bone);
    bsSend.Write(offset);
    bsSend.Write(rot);
    bsSend.Write(scale);
    bsSend.Write((uint32_t)0);
    bsSend.Write((uint32_t)0);

    pNetGame->GetRakClient()->RPC(&RPC_EditAttachedObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AttachEdit_AttachClick(JNIEnv *env, jobject thiz, jint button_type,
                                                  jboolean button_id) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CEditobject::iEditedSlot;

    if (!pPlayer->m_aAttachedObjects[slot].bState) return;
   // CObject* pObject = pPlayer->m_aAttachedObjects[slot].pObject;
    if (pPlayer->IsAdded())
    {
        if(button_type == 1) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecOffset.X +=0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecOffset.X -=0.005;
            }
        }
        if(button_type == 0) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecOffset.Z +=0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecOffset.Z -=0.005;
            }
        }
        if(button_type == 2) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecOffset.Y +=0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecOffset.Y -=0.005;
            }
        }
        if(button_type == 3) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecScale.X += 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.Y += 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.Z += 0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecScale.X -= 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.Y -= 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.Z -= 0.005;
            }
        }
        if(button_type == 4) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecRotation.X +=1;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecRotation.X -=1;
            }
        }
        if(button_type == 5) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecRotation.Y +=1;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecRotation.Y -=1;
            }
        }
        if(button_type == 6) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecRotation.Z +=1;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecRotation.Z -=1;
            }
        }
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AttachEdit_Save(JNIEnv *env, jobject thiz) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CEditobject::iEditedSlot;

    CEditobject::bIsToggle = false;
    CEditobject::iEditedSlot = INVALID_EDITED_SLOT;
    CEditobject::SendOnEditAttach(
            1,
            slot,
            pPlayer->m_aAttachedObjects[slot].dwModelId,
            pPlayer->m_aAttachedObjects[slot].dwBone,
            pPlayer->m_aAttachedObjects[slot].vecOffset,
            pPlayer->m_aAttachedObjects[slot].vecRotation,
            pPlayer->m_aAttachedObjects[slot].vecScale
    );
}
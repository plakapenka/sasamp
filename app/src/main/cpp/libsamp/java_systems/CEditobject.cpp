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
jobject CEditobject::thiz = nullptr;
jclass CEditobject::clazz = nullptr;

extern CJavaWrapper *g_pJavaWrapper;

void CEditobject::StartEditAttachedObject(int slot)
{
    if(!pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->IsValidAttach(slot)){
        CChatWindow::AddDebugMessage("Invalid attach slot %d", slot);
        return;
    }
    CEditobject::iEditedSlot = slot;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if(CEditobject::thiz == nullptr)
    {
        jmethodID constructor = env->GetMethodID(CEditobject::clazz, "<init>","(Landroid/app/Activity;)V");
        CEditobject::thiz = env->NewObject(CEditobject::clazz, constructor, g_pJavaWrapper->activity);
        CEditobject::thiz = env->NewGlobalRef(CEditobject::thiz);
    }

    bIsToggle = true;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AttachEdit_Exit(JNIEnv *env, jobject thiz) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CEditobject::iEditedSlot;

    CEditobject::iEditedSlot = INVALID_EDITED_SLOT;
    CEditobject::SendOnEditAttach(
            0,
            slot,
            pPlayer->m_aAttachedObjects[slot].dwModelId,
            pPlayer->m_aAttachedObjects[slot].dwSampBone,
            pPlayer->m_aAttachedObjects[slot].vecOffset,
            pPlayer->m_aAttachedObjects[slot].vecRotation,
            pPlayer->m_aAttachedObjects[slot].vecScale
            );

    CEditobject::bIsToggle = false;
    env->DeleteGlobalRef( CEditobject::thiz );
    CEditobject::thiz = nullptr;
}

void CEditobject::SendOnEditAttach(int response, int index, int modelid, int bone, CVector offset, CVector rot, CVector scale){

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
                pPlayer->m_aAttachedObjects[slot].vecOffset.x +=0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecOffset.x -=0.005;
            }
        }
        if(button_type == 0) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecOffset.z +=0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecOffset.z -=0.005;
            }
        }
        if(button_type == 2) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecOffset.y +=0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecOffset.y -=0.005;
            }
        }
        if(button_type == 3) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecScale.x += 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.y += 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.z += 0.005;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecScale.x -= 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.y -= 0.005;
                pPlayer->m_aAttachedObjects[slot].vecScale.z -= 0.005;
            }
        }
        if(button_type == 4) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecRotation.x +=1;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecRotation.x -=1;
            }
        }
        if(button_type == 5) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecRotation.y +=1;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecRotation.y -=1;
            }
        }
        if(button_type == 6) {
            if(button_id) {
                pPlayer->m_aAttachedObjects[slot].vecRotation.z +=1;
            }else {
                pPlayer->m_aAttachedObjects[slot].vecRotation.z -=1;
            }
        }
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AttachEdit_Save(JNIEnv *env, jobject thiz) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CEditobject::iEditedSlot;

    CEditobject::iEditedSlot = INVALID_EDITED_SLOT;
    CEditobject::SendOnEditAttach(
            1,
            slot,
            pPlayer->m_aAttachedObjects[slot].dwModelId,
            pPlayer->m_aAttachedObjects[slot].dwSampBone,
            pPlayer->m_aAttachedObjects[slot].vecOffset,
            pPlayer->m_aAttachedObjects[slot].vecRotation,
            pPlayer->m_aAttachedObjects[slot].vecScale
    );

    CEditobject::bIsToggle = false;

    env->DeleteGlobalRef( CEditobject::thiz );
    CEditobject::thiz = nullptr;
}
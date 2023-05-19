//
// Created by plaka on 11.02.2023.
//

#include "CSpeedometr.h"
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "util/CJavaWrapper.h"

jclass  CSpeedometr::clazz = nullptr;
jobject CSpeedometr::thiz = nullptr;
bool    CSpeedometr::bIsShow = false;
bool    CSpeedometr::bIsNoNeedDraw = false;
float   CSpeedometr::fFuel = 0.0f;
int     CSpeedometr::iMilliage = 0;

void CSpeedometr::show()
{
    if( CSpeedometr::bIsShow ) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();
    // construct

    jmethodID constructor = env->GetMethodID(CSpeedometr::clazz, "<init>", "(Landroid/app/Activity;)V");
    CSpeedometr::thiz = env->NewObject(CSpeedometr::clazz, constructor, g_pJavaWrapper->activity);
    CSpeedometr::thiz = env->NewGlobalRef(CSpeedometr::thiz);

    CSpeedometr::bIsShow = true;
}

void CSpeedometr::tempToggle(bool toggle)
{
    if( !CSpeedometr::bIsShow ) return;

    bIsNoNeedDraw = !toggle;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(CSpeedometr::clazz, "tempToggle", "(Z)V");
    env->CallVoidMethod(CSpeedometr::thiz, method, toggle);
}

void CSpeedometr::hide()
{
    if( !CSpeedometr::bIsShow ) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetMethodID(CSpeedometr::clazz, "destructor", "()V");
    env->CallVoidMethod(CSpeedometr::thiz, method);

    env->DeleteGlobalRef(CSpeedometr::thiz);

    CSpeedometr::bIsShow = false;
}

void CSpeedometr::update()
{
    if( !CSpeedometr::bIsShow ) return;

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
    if(!pPlayer)return;

    CPlayerPed *pPed = pPlayer->GetPlayerPed();
    if(!pPed) return;

    if(!pPed->IsInVehicle()) return;

    CVehicle* pVehicle = pPed->GetCurrentVehicle();

    jmethodID method = env->GetMethodID(clazz, "update", "(IIIIIIII)V");
    env->CallVoidMethod(
            CSpeedometr::thiz,
            method,
            (int) (pVehicle->m_pVehicle->pHandling->m_transmissionData.m_fCurrentSpeed * 175.0f),
            (int) CSpeedometr::fFuel,
            (int) pVehicle->GetHealth(),
            CSpeedometr::iMilliage,
            pVehicle->m_bEngineOn,
            (int) pVehicle->GetLightsState(),
            (int) pVehicle->m_bIsLocked,
            pVehicle->m_iTurnState
            );
}



extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_Speedometer_sendClick(JNIEnv *env, jobject thiz, jint click_id) {
    switch(click_id)
    {
        case CSpeedometr::BUTTON_ENGINE:
        {
            pNetGame->SendChatCommand("/e");
            break;
        }
        case CSpeedometr::BUTTON_LIGHT:
        {
            pNetGame->SendChatCommand("/light");
            break;
        }
        case CSpeedometr::BUTTON_TURN_LEFT:
        {
            CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_LEFT)
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_LEFT;

            break;
        }
        case CSpeedometr::BUTTON_TURN_RIGHT:
        {
            CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_RIGHT)
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_RIGHT;

            break;
        }
        case CSpeedometr::BUTTON_TURN_ALL:
        {
            CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
            CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

            if(pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_ALL)
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
            else
                pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_ALL;

            break;
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ssmp_cr_gui_tunings_Colors_color_1change(JNIEnv *env, jobject thiz, jint type,
                                                        jint r, jint g, jint b) {
    if(!pNetGame)
        return;

    if(!pNetGame->GetPlayerPool())
        return;

    if(!pNetGame->GetPlayerPool()->GetLocalPlayer())
        return;

    CPlayerPed *pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->m_pPlayerPed;
    CVehicle* pVehicle = pPlayerPed->GetCurrentVehicle();

    if(pVehicle) {
        pVehicle->color1.R = r;
        pVehicle->color1.G = g;
        pVehicle->color1.B = b;
    }

}
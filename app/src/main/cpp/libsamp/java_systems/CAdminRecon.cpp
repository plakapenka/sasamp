//
// Created by plaka on 10.01.2023.
//

#include "CAdminRecon.h"
#include <jni.h>

#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"

#include "CHUD.h"

jobject j_mAdminRecon;

bool CAdminRecon::bIsToggle = false;
PLAYERID CAdminRecon::iPlayerID = INVALID_PLAYER_ID;

void CAdminRecon::hide(){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    jmethodID Hide = env->GetMethodID(clazz, "hide", "()V");

    env->CallVoidMethod(j_mAdminRecon, Hide);

    CAdminRecon::bIsToggle = false;
    CAdminRecon::iPlayerID = INVALID_PLAYER_ID;
}

void CAdminRecon::tempToggle(bool toggle){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    jmethodID tempToggle = env->GetMethodID(clazz, "tempToggle", "(Z)V");

    env->CallVoidMethod(j_mAdminRecon, tempToggle, toggle);
}

void CAdminRecon::show(int targetId){
    JNIEnv* env = g_pJavaWrapper->GetEnv();
    if(!env)return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

    if(!pPlayerPool->m_pPlayers[targetId])return;

    jstring jName = env->NewStringUTF( pPlayerPool->GetPlayerName(targetId) );

    jclass clazz = env->GetObjectClass(j_mAdminRecon);
    jmethodID Show = env->GetMethodID(clazz, "show", "(Ljava/lang/String;I)V");

    env->CallVoidMethod(j_mAdminRecon, Show, jName, targetId);
    env->DeleteLocalRef(jName);

    CAdminRecon::bIsToggle = true;
    CAdminRecon::iPlayerID = targetId;
}

void CNetGame::packetAdminRecon(Packet* p)
{
    RakNet::BitStream bs((unsigned char*)p->data, p->length, false);

    bs.IgnoreBits(40); // skip packet and rpc id

    uint8_t toggle;
    uint32_t targetID;

    bs.Read(toggle);
    bs.Read(targetID);

    if(toggle == 1){
        CAdminRecon::show((int)targetID);
    } else {
        CAdminRecon::hide();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AdminRecon_init(JNIEnv *env, jobject thiz) {
    j_mAdminRecon = env->NewGlobalRef(thiz);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AdminRecon_clickButton(JNIEnv *env, jobject thiz, jint button_id) {
    switch (button_id) {
        case CAdminRecon::Buttons::EXIT_BUTTON:{
            pNetGame->SendChatCommand("/reoff");
            CAdminRecon::bIsToggle = false;
            break;
        }
        case CAdminRecon::Buttons::STATS_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/getstats %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::FREEZE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/freeze %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::UNFREEZE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/unfreeze %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::SPAWN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/spawn %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::SLAP_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/slap %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::REFRESH_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::MUTE_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/mute %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::JAIL_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/jail %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::KICK_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/kick %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::BAN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/ban %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::WARN_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/warn %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::NEXT_BUTTON:{

            PLAYERID playerid = CAdminRecon::iPlayerID + 1;
            while(!pNetGame->GetPlayerPool()->GetAt(playerid))
            {
                playerid++;
                if(playerid > MAX_PLAYERS) playerid = 0;
            }
            CAdminRecon::iPlayerID = playerid;

            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::PREV_BUTTON:{

            PLAYERID playerid = CAdminRecon::iPlayerID - 1;
            while(!pNetGame->GetPlayerPool()->GetAt(playerid))
            {
                playerid--;
                if(playerid < 0) playerid = MAX_PLAYERS;
            }
            CAdminRecon::iPlayerID = playerid;

            char tmp[255];
            sprintf(tmp, "/re %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        case CAdminRecon::Buttons::FLIP_BUTTON:{
            char tmp[255];
            sprintf(tmp, "/flip %d", CAdminRecon::iPlayerID);
            pNetGame->SendChatCommand(tmp );
            break;
        }
        default: return;
    }
}
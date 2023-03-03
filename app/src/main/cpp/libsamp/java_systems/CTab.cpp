#include "main.h"
#include "CTab.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"
#include "CHUD.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

extern CNetGame* pNetGame;
extern CGame* pGame;
extern CGUI* pGUI;

jobject CTab::thiz = nullptr;

bool CTab::bIsShow = false;

void CTab::toggle()
{
    CTab::bIsShow = !CTab::bIsShow;
    if (CTab::bIsShow)
    {
      //  TabUpdate();

        // Get player list
        pNetGame->UpdatePlayerScoresAndPings();

    }
}

void CTab::update() {
    // Get player list
   // pNetGame->UpdatePlayerScoresAndPings();

    CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

    CTab::setStat(pPlayerPool->GetLocalPlayerID(),
                               pPlayerPool->GetLocalPlayerName(),
                               pPlayerPool->GetLocalPlayerScore(),
                               pPlayerPool->GetLocalPlayerPing());

    PLAYERID i = 1, x;
    for (x = 0; x < MAX_PLAYERS; x++)
    {
        if (!pPlayerPool->m_pPlayers[x]) continue;

        CTab::setStat(x,
                pPlayerPool->GetPlayerName(x),
                pPlayerPool->GetRemotePlayerScore(x),
                pPlayerPool->GetRemotePlayerPing(x) );

    }

    //Show Window
    CTab::show();
}


void CTab::show()
{
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass Tab = env->GetObjectClass(CTab::thiz);

    jmethodID show = env->GetMethodID(Tab, "show", "()V");
    env->CallVoidMethod(CTab::thiz, show);
}

void CTab::setStat(int id, char name[], int score, int ping) {

    JNIEnv* env = g_pJavaWrapper->GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jclass Tab = env->GetObjectClass(CTab::thiz);
    jmethodID setStat = env->GetMethodID(Tab, "setStat", "(ILjava/lang/String;II)V");


    jstring jPlayerName = env->NewStringUTF( name );

    env->CallVoidMethod(CTab::thiz, setStat, id, jPlayerName, score, ping);
    env->DeleteLocalRef(jPlayerName);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_tab_Tab_onTabClose(JNIEnv *env, jobject thiz) {
    CTab::toggle();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_tab_Tab_initTab(JNIEnv *env, jobject thiz) {
    CTab::thiz = env->NewGlobalRef(thiz);
}
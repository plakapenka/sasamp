#include "main.h"
#include "scoreboard.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "vendor/imgui/imgui_internal.h"
#include "util/CJavaWrapper.h"
#include "CSettings.h"
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

extern CNetGame* pNetGame;
extern CGame* pGame;
extern CGUI* pGUI;
extern CSettings* pSettings;

jobject jTab;

bool tabToggle = false;

void ToggleTab()
{
    tabToggle = !tabToggle;
    if (tabToggle)
    {
        TabUpdate();

        // Freeze player
        pGame->ToggleAllHud(false);

        // Get player list
        pNetGame->UpdatePlayerScoresAndPings();

        //Show Window
        g_pJavaWrapper->ShowTabWindow();


    }
    else
    {
        // Unfreeze player
        pGame->ToggleAllHud(true, false, true);
    }

}

void TabUpdate() {
    // Get player list
    pNetGame->UpdatePlayerScoresAndPings();

    CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

    g_pJavaWrapper->SetTabStat(pPlayerPool->GetLocalPlayerID(),
                               pPlayerPool->GetLocalPlayerName(),
                               pPlayerPool->GetLocalPlayerScore(),
                               pPlayerPool->GetLocalPlayerPing());

    PLAYERID i = 1, x;
    for (x = 0; x < MAX_PLAYERS; x++)
    {
        if (!pPlayerPool->GetSlotState(x)) continue;

        g_pJavaWrapper->SetTabStat(x,
                                   pPlayerPool->GetPlayerName(x),
                                   pPlayerPool->GetRemotePlayerScore(x),
                                   pPlayerPool->GetRemotePlayerPing(x));

    }
}


void CJavaWrapper::ShowTabWindow()
{
    JNIEnv* env = GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }
    jclass Tab = env->GetObjectClass(jTab);

    jmethodID show = env->GetMethodID(Tab, "show", "()V");
    env->CallVoidMethod(jTab, show);
}

void CJavaWrapper::SetTabStat(int id, char name[], int score, int ping) {

    JNIEnv* env = GetEnv();

    if (!env)
    {
        Log("No env");
        return;
    }

    jclass Tab = env->GetObjectClass(jTab);
    jmethodID setStat = env->GetMethodID(Tab, "setStat", "(ILjava/lang/String;II)V");


    jstring jPlayerName = env->NewStringUTF( name );

    env->CallVoidMethod(jTab, setStat, id, jPlayerName, score, ping);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_tab_Tab_onTabClose(JNIEnv *env, jobject thiz) {
    ToggleTab();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_tab_Tab_initTab(JNIEnv *env, jobject thiz) {
    jTab = env->NewGlobalRef(thiz);
}
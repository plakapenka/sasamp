//
// Created by plaka on 24.01.2023.
//

#include "CLoader.h"
#include "game/bass.h"
#include "util/patch.h"
#include "crashlytics.h"
#include "CSettings.h"


void InitBASSFuncs();
void CLoader::loadBassLib()
{
    InitBASSFuncs();
    BASS_Init(-1, 44100, BASS_DEVICE_3D, nullptr, nullptr);
    BASS_Set3DFactors(0.2, 0.1, 0);
}

void CLoader::redirectDirection(const char* path)
{
    strcpy(g_pszStorage, path);

    CHook::UnFuck(g_libGTASA + 0x63C4B8);
    *(char**)(g_libGTASA + 0x63C4B8) = (char*)g_pszStorage;

    CHook::UnFuck(g_libGTASA + 0x5D1608);
    *(char**)(g_libGTASA + 0x5D1608) = (char*)g_pszStorage;

    Log("Storage: %s", g_pszStorage);

    if( !strlen(g_pszStorage) )
    {
        Log("Error: storage path not found! %s", path);
        std::terminate();
    }
}

void CLoader::initCrashLytics()
{
    firebase::crashlytics::SetCustomKey("build data", __DATE__);
    firebase::crashlytics::SetCustomKey("build time", __TIME__);

    firebase::crashlytics::SetUserId(CSettings::m_Settings.szNickName);
    firebase::crashlytics::SetCustomKey("Nick", CSettings::m_Settings.szNickName);

    uintptr_t libsamp = FindLibrary("libsamp.so");
    uintptr_t libc = FindLibrary("libc.so");

    char str[100];

    sprintf(str, "0x%x", g_libGTASA);
    firebase::crashlytics::SetCustomKey("libGTASA.so", str);

    sprintf(str, "0x%x", libsamp);
    firebase::crashlytics::SetCustomKey("libsamp.so", str);

    sprintf(str, "0x%x", libc);
    firebase::crashlytics::SetCustomKey("libc.so", str);
}
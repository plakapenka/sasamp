//
// Created by plaka on 24.03.2023.
//

#include <cstdint>
#include <algorithm>
#include "Timer.h"
#include "oswrapper/oswrapper.h"
#include "util/patch.h"
#include "java_systems/CSpeedometr.h"
#include "util/CJavaWrapper.h"
#include "keyboard.h"

float CTimer::game_FPS = 0;

bool CTimer::m_CodePause = false;
bool CTimer::m_UserPause = false;
float CTimer::ms_fTimeScale = 0;
uint32_t CTimer::m_FrameCounter = 0;
uint32_t CTimer::m_snTimeInMilliseconds = 0;
bool CTimer::bSkipProcessThisFrame = false;
float CTimer::ms_fTimeStep = 0;

void CTimer::InjectHooks()
{
    CHook::Write(g_libGTASA + 0x006794BC, &CTimer::m_CodePause);
    CHook::Write(g_libGTASA + 0x0067892C, &CTimer::m_FrameCounter);
    CHook::Write(g_libGTASA + 0x00677674, &CTimer::game_FPS);
    CHook::Write(g_libGTASA + 0x006776A8, &CTimer::m_UserPause);
    CHook::Write(g_libGTASA + 0x0067689C, &CTimer::ms_fTimeScale);
    CHook::Write(g_libGTASA + 0x00676FF0, &CTimer::m_snTimeInMilliseconds);
    CHook::Write(g_libGTASA + 0x00679248, &CTimer::bSkipProcessThisFrame);
    CHook::Write(g_libGTASA + 0x0067770C, &CTimer::ms_fTimeStep);

    CHook::Redirect(g_libGTASA, 0x00421118, &CTimer::StartUserPause);
    CHook::Redirect(g_libGTASA, 0x00421128, &CTimer::EndUserPause);
}


uint64_t GetMillisecondTime() {

}

// 0x5617E0
void CTimer::Initialise()
{

}

// 0x5618C0
void CTimer::Shutdown() {

}

// 0x5619D0
void CTimer::Suspend()
{

}

// 0x561A00
void CTimer::Resume()
{

}

// 0x561AA0
void CTimer::Stop()
{

}

// 0x561AF0
void CTimer::StartUserPause()
{
    if (g_pJavaWrapper)
    {
        CKeyBoard::Close();
        g_pJavaWrapper->SetPauseState(true);
        CSpeedometr::tempToggle(false);
    }
    m_UserPause = true;
}

// 0x561B00
void CTimer::EndUserPause()
{
    // process resume event
    if (g_pJavaWrapper)
    {
        g_pJavaWrapper->SetPauseState(false);
        CSpeedometr::tempToggle(true);
    }
    m_UserPause = false;
}

// 0x561A40
uint32_t CTimer::GetCyclesPerMillisecond()
{
    return CHook::CallFunction<uint32_t>(g_libGTASA + 0x0042100C + 1);
}

// cycles per ms * 20
// 0x561A50
uint32_t CTimer::GetCyclesPerFrame()
{

}

uint64_t CTimer::GetCurrentTimeInCycles()
{
    return CHook::CallFunction<uint64_t>(g_libGTASA + 0x00421040 + 1);
}

// 0x561AD0
bool CTimer::GetIsSlowMotionActive()
{

}

// 0x5618D0
void CTimer::UpdateVariables(float timeElapsed)
{

}

// 0x561B10
void CTimer::Update()
{

}

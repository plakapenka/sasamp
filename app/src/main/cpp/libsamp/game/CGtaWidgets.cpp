//
// Created by plaka on 03.02.2023.
//

#include "CGtaWidgets.h"
#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "util/patch.h"
#include "java_systems/CHUD.h"

uintptr_t* CGtaWidgets::pWidgets = nullptr; // int CTouchInterface::m_pWidgets[]
extern CNetGame *pNetGame;

void (*CWidgetButton__Draw)(int thiz);
void CWidgetButton__Draw_hook(int thiz)
{
    if(!CHUD::bIsShow)return;

    if(thiz == CGtaWidgets::pWidgets[WIDGET_POSITION_HORN]) {
        return;
    }

    CWidgetButton__Draw(thiz);
}

void CGtaWidgets::setEnabled(int thiz, bool bEnabled)
{
    *(BYTE *)(thiz + 0x4D) = bEnabled; // this->m_bEnabled
}

//bool (*CWidget__IsTouched)(uintptr_t* thiz, CVector2D *pVecOut);
//bool CWidget__IsTouched_hook(uintptr_t* thiz, CVector2D *pVecOut)
//{
//    if(*thiz == CGtaWidgets::pWidgets[WIDGET_POSITION_HORN]) {
//        return true;
//    }
//    return CWidget__IsTouched(thiz, pVecOut);
//}

void (*CWidgetButton__Update)(int result, int a2, int a3, int a4);
void CWidgetButton__Update_hook(int result, int a2, int a3, int a4)
{
    if (!result)
    {
        return;
    }

    CGtaWidgets::setEnabled(CGtaWidgets::pWidgets[WIDGET_POSITION_ENTER_CAR], false);

    if(pNetGame && pNetGame->m_GreenZoneState )
    {
        CGtaWidgets::setEnabled(CGtaWidgets::pWidgets[WIDGET_POSITION_CAR_SHOOT], false);
    }
    CWidgetButton__Update(result, a2, a3, a4);
}

void CGtaWidgets::init()
{
    CGtaWidgets::pWidgets = (uintptr_t*)(g_libGTASA + 0x00657E48);

   // CHook::InlineHook(g_libGTASA, 0x00274218, &CWidget__IsTouched_hook, &CWidget__IsTouched);
    CHook::InlineHook(g_libGTASA, 0x00274AB4, &CWidgetButton__Update_hook, &CWidgetButton__Update);
    CHook::SetUpHook(g_libGTASA + 0x00274748, (uintptr_t)CWidgetButton__Draw_hook, (uintptr_t*)&CWidgetButton__Draw);
}



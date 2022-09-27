#include "main.h"
#include "crosshair.h"
#include "../gui/gui.h"
#include "game.h"
#include "../net/netgame.h"

extern CGame* pGame;
extern CNetGame* pNetGame;


CCrossHair::CCrossHair(/* args */)
{   
    SetTexture(LoadTextureFromDB("txd", "siteM16"));
    m_bNeedFix = false;
}

CCrossHair::~CCrossHair()
{

}

void CCrossHair::Render()
{   
    bool v2; // zf
  int v3; // r0
  bool v4; // zf
  unsigned int v5; // r0
  float v6; // s18
  int v7; // r6
  int v8; // s0
  float v9; // s0
  float v10; // s0
  float (*v11)(PED_TYPE*); // r6
  PED_TYPE* v12; // r0
  float v13; // s20
  double v14; // d1
  float v15; // s24
  float v16; // s22
  float v17; // s18
  float v18; // s16
  float v19; // s26
  float v20; // s20
  char v22[4]; // [sp+8h] [bp-58h] BYREF
  RECT v23; // [sp+Ch] [bp-54h] BYREF
  float v24; // [sp+10h] [bp-50h]
  float v25; // [sp+14h] [bp-4Ch]
  float v26; // [sp+18h] [bp-48h]
  int v27; // [sp+1Ch] [bp-44h]

      v5 = *(uint16_t *)(g_libGTASA + 528 * *(char *)(g_libGTASA + 0x8B085F) + 0x8B0986) - 39;
      if ( v5 <= 14 && ((1 << v5) & 0x400B) != 0 )
      {
        v6 = (double)(16 * (RsGlobal->maximumHeight / 9)) * 0.524 + (double)((RsGlobal->maximumWidth - 16 * (RsGlobal->maximumHeight / 9)) / 2);
        //Log("fCHairScreenMultX: %f", v6);
        if ( v6 <= 1080.0 || v6 >= 1920.0 )
        {
          m_bNeedFix = false;
        }
        else
        {
          m_bNeedFix = true;
          v7 = RsGlobal->maximumHeight / 9;
          v8 = RsGlobal->maximumWidth / (RsGlobal->maximumWidth - 16 * v7);
          v9 = (double)v8 + (double)v8;
          if ( v9 < 0.0 )
            v9 = 0.0;
          v10 = (double)(6 * (16 * v7 / 10)) * 0.4 + v9;
          //Log("fCHairScreenMultY: %f", v10);
          v11 = (float (*)(PED_TYPE*))(g_libGTASA + 0x454F61);
          v12 = GamePool_FindPlayerPed();
          v13 = (double)RsGlobal->maximumHeight / 448.0 * 64.0 * v11(v12);
          v14 = v13 * 0.5;
          v15 = v14 + v6 - v13;
          v16 = v14 + v10 - v13;
          v17 = v14 + v16;
          v18 = v14 + v15;
          Draw( RECT{v15, v17, v18, v16}, CRGBA(0xFFu, 0xFFu, 0xFFu, 0xFFu));
          v19 = v13 + v15;
          Draw( RECT{v15 + v13, v17, v18, v16}, CRGBA(0xFFu, 0xFFu, 0xFFu, 0xFFu));
          v20 = v13 + v16;
          Draw( RECT{v15, v17, v18, v20}, CRGBA(0xFFu, 0xFFu, 0xFFu, 0xFFu));
          Draw( RECT{v19, v17, v18, v20}, CRGBA(0xFFu, 0xFFu, 0xFFu, 0xFFu));
        }
      }
}
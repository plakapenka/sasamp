#pragma once
#include "font.h"
#include <string>

class CSkyBox
{
	static bool m_bEnabled;
	static bool m_bPendingStatus;
	
	static CObject* CreateObjectScaled(int iModel, float fScale);
public:
	static CObject* m_pSkyObjectDaily;
	static CObject* m_pSkyObjectNight;
	static CObject* m_pSkyObjectEvening;
	static CObject* m_pSkyObjectAfternoon;

	static void SetEnabled(bool bEnabled);
	static void Process();
};
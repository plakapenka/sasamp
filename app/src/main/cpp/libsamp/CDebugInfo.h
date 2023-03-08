#pragma once

#include <stdint.h>

#define DEBUG_INFO_ENABLED

class CDebugInfo
{

public:
	static uint32_t uiStreamedPeds;
	static uint32_t uiStreamedVehicles;

	static void SetDrawFPS(bool bDraw);

	static void Draw();

	static uint32_t m_uiDrawDebug;
};


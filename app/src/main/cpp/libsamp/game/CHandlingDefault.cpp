#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"

tHandlingData CHandlingDefault::m_aDefaultModelHandlings[MAX_VEHICLE_MODELS];
bool CHandlingDefault::bIsSlotUsed[MAX_VEHICLE_MODELS]{false};

void CHandlingDefault::GetDefaultHandling(uint16_t usHandlingID, tHandlingData* tDest)
{
	Log("copy handling %d", usHandlingID);

	memcpy((void*)tDest, &m_aDefaultModelHandlings[usHandlingID], sizeof(tHandlingData));
}

void CHandlingDefault::FillDefaultHandling(uint16_t usHandlingID, tHandlingData* pSrc)
{
	if(bIsSlotUsed[usHandlingID]) return;

    bIsSlotUsed[usHandlingID] = true;

	memset(&m_aDefaultModelHandlings[usHandlingID], 0, sizeof(tHandlingData));

	if (pSrc)
	{
		memcpy(&m_aDefaultModelHandlings[usHandlingID], pSrc, sizeof(tHandlingData));
	}
}

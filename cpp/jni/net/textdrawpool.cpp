#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../gui/gui.h"
#include "../keyboard.h"

extern CGame* pGame;
extern CNetGame* pNetGame;
extern CGUI* pGUI;
extern CKeyBoard* pKeyBoard;

//----------------------------------------------------

CTextDrawPool::CTextDrawPool()
{

	int x = 0;
	while (x != MAX_TEXT_DRAWS) {
		m_pTextDraw[x] = NULL;
		m_bSlotState[x] = FALSE;
		x++;
	}
	m_bSelectState = false;
}

//----------------------------------------------------

CTextDrawPool::~CTextDrawPool()
{

	int x = 0;
	while (x != MAX_TEXT_DRAWS)
	{
		if (m_pTextDraw[x]) {
			delete m_pTextDraw[x];
			m_pTextDraw[x] = NULL;
			m_bSlotState[x] = FALSE;
		}
		x++;
	}
}

//----------------------------------------------------

CTextDraw* CTextDrawPool::New(uint16_t wText, TEXT_DRAW_TRANSMIT* TextDrawTransmit, char* szText)
{
	if (wText > MAX_TEXT_DRAWS || wText < 0) return nullptr;
	if (m_pTextDraw[wText]) Delete(wText);

	CTextDraw* pTextDraw = new CTextDraw(TextDrawTransmit, szText);

	if (pTextDraw)
	{

		m_pTextDraw[wText] = pTextDraw;
		m_bSlotState[wText] = TRUE;

		return pTextDraw;
	}
	return NULL;
}

//----------------------------------------------------

void CTextDrawPool::Delete(uint16_t wText)
{
	if (m_pTextDraw[wText])
	{

		delete m_pTextDraw[wText];
		m_pTextDraw[wText] = NULL;
		m_bSlotState[wText] = FALSE;
	}
}

//----------------------------------------------------

void CTextDrawPool::Draw()
{
	int x = 0;

	while (x != MAX_TEXT_DRAWS) {
		if (m_bSlotState[x])
		{
			m_pTextDraw[x]->Draw();
		}
		x++;
	}
}

bool CTextDrawPool::OnTouchEvent(int type, bool multi, int x, int y)
{

    if(pKeyBoard)
    {
        if(pKeyBoard->IsOpen()) 
            return true;
    }

    m_wClickedTextDrawID = 0xFFFF;
    for(int i = 0; i < MAX_TEXT_DRAWS; i++)
    {
        if(m_bSlotState[i] && m_pTextDraw[i])
        {
            CTextDraw* pTextDraw = m_pTextDraw[i];
            pTextDraw->m_bHovered = false;
            pTextDraw->m_dwHoverColor = 0;
 
            if(pTextDraw->m_TextDrawData.byteSelectable)
            {
                if(IsPointInRect(x, y, &pTextDraw->m_rectArea))
                {
                    switch(type)
                    {
                        case TOUCH_PUSH:
                        case TOUCH_MOVE:
                        m_wClickedTextDrawID = 0xFFFF;
                        pTextDraw->m_bHovered = true;
                        pTextDraw->m_dwHoverColor = m_dwHoverColor;
                        break;
 
                        case TOUCH_POP:
                        m_wClickedTextDrawID = i;
                        SendClickTextDraw();
                        pTextDraw->m_bHovered = false;
                        pTextDraw->m_dwHoverColor = 0;
                        break;
                    }
                }
            }
        }
    }

	return false;
}

void CTextDrawPool::SetSelectState(bool bState, uint32_t dwColor)
{
		if(bState) 
    {
		m_bSelectState = true;
		m_dwHoverColor = dwColor;
        m_wClickedTextDrawID = 0xFFFF;
		//pGame->DisplayHUD(false);
		//pGame->FindPlayerPed()->TogglePlayerControllableWithoutLock(false);
	}
	else 
    {
		m_bSelectState = false;
		m_dwHoverColor = 0;
		m_wClickedTextDrawID = 0xFFFF;
		//pGame->DisplayHUD(true);
		//pGame->FindPlayerPed()->TogglePlayerControllableWithoutLock(true);

		//SendClickTextDraw();

		for(int i = 0; i < MAX_TEXT_DRAWS; i++)
		{
			if(m_bSlotState[i] && m_pTextDraw[i]) 
            {
				CTextDraw* pTextDraw = m_pTextDraw[i];
				pTextDraw->m_bHovered = false;
				pTextDraw->m_dwHoverColor = 0;
			}
		}
	}
}

void CTextDrawPool::SendClickTextDraw()
{
	RakNet::BitStream bsSend;
	bsSend.Write(m_wClickedTextDrawID);
	pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0);
}

void CTextDrawPool::SnapshotProcess()
{
	for (int i = 0; i < MAX_TEXT_DRAWS; i++)
	{
		if (m_bSlotState[i] && m_pTextDraw[i]) {
			m_pTextDraw[i]->SnapshotProcess();
		}
	}
}

//----------------------------------------------------
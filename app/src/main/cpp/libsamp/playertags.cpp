#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "playertags.h"
#include "CSettings.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;

CPlayerTags::CPlayerTags()
{
	m_pAfk_icon = CUtil::LoadTextureFromDB("samp", "afk_icon");
//	m_pVoice_icon = (RwTexture*)LoadTextureFromDB("samp", "micro_icon");

//	m_pKeyboard_icon = (RwTexture*)LoadTextureFromDB("samp", "keyboard_icon");

	HealthBarBDRColor = ImColor( 0x00, 0x00, 0x00, 0xFF );

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_bChatBubbleStatus[i] = 0;
		m_pSzText[i] = nullptr;
		m_pSzTextWithoutColors[i] = nullptr;
		m_iVoiceTime[i] = 0;
		m_iLastVoiceTimeUpdated[i] = 0;
	}
}

CPlayerTags::~CPlayerTags() {}

#include <algorithm>
void CPlayerTags::Render()
{
	CVector VecPos;
	MATRIX4X4 matLocal, matPlayer;
	int dwHitEntity;
	char szNickBuf[50];

	ImVec2 basePos = ImVec2(pGUI->ScaleX(10.0f), pGUI->ScaleY(600.0f));
	if(pNetGame && pNetGame->m_bShowPlayerTags)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		pGame->FindPlayerPed()->GetMatrix(&matLocal);

		for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
			if (pPlayerPool->GetSlotState(playerId) == true) {
				CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);

				if (pPlayer && pPlayer->IsActive() && pPlayer->m_bShowNameTag) {
					CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();

					if (m_bChatBubbleStatus[playerId]) {
						if (!pPlayerPed) {
							ResetChatBubble(playerId);
							continue;
						}
						if (pPlayerPed->GetDistanceFromCamera() <= m_fDistance[playerId]) {
							if (!pPlayerPed->IsAdded()) continue;
							VecPos.x = 0.0f;
							VecPos.y = 0.0f;
							VecPos.z = 0.0f;
							pPlayerPed->GetBonePosition(8, VecPos);
							DrawChatBubble(playerId, &VecPos, pPlayerPed->GetDistanceFromCamera());
						}
						if (GetTickCount() - m_dwStartTime[playerId] >= m_dwTime[playerId]) {
							ResetChatBubble(playerId);
						}
					}

					if (pPlayerPed->GetDistanceFromCamera() <= pNetGame->m_fNameTagDrawDistance) {

						{
							if (!pPlayerPed->IsAdded()) continue;
							VecPos.x = 0.0f;
							VecPos.y = 0.0f;
							VecPos.z = 0.0f;
							pPlayerPed->GetBonePosition(8, VecPos);
						}

						CAMERA_AIM *pCam = GameGetInternalAim();
						dwHitEntity = 0;

						if (pNetGame->m_bNameTagLOS) {
							dwHitEntity = ScriptCommand(&get_line_of_sight,
														VecPos.x, VecPos.y, VecPos.z,
														pCam->pos1x, pCam->pos1y, pCam->pos1z,
														1, 0, 0, 1, 0);
						}

						if (!pNetGame->m_bNameTagLOS || dwHitEntity) {

							sprintf(szNickBuf, "%s (%d)", pPlayerPool->GetPlayerName(playerId),
									playerId);
							Draw(&VecPos, szNickBuf,
								 pPlayer->GetPlayerColor(),
								 pPlayerPed->GetDistanceFromCamera(),
								 pPlayer->m_fCurrentHealth,
								 pPlayer->m_fCurrentArmor,
								 pPlayer->IsAFK(), 0, pPlayer->m_bKeyboardOpened);
						}
					}
				}
			}
		}
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors = nullptr);

void FilterColors(char* szStr);

void CPlayerTags::AddChatBubble(PLAYERID playerId, char* szText, uint32_t dwColor, float fDistance, uint32_t dwTime)
{
	if (m_bChatBubbleStatus[playerId])
	{
		ResetChatBubble(playerId);
		m_dwColors[playerId] = dwColor;
		m_fDistance[playerId] = fDistance;
		m_dwTime[playerId] = dwTime;
		m_dwStartTime[playerId] = GetTickCount();
		m_bChatBubbleStatus[playerId] = 1;
		m_fTrueX[playerId] = -1.0f;
		cp1251_to_utf8(m_pSzText[playerId], szText);
		cp1251_to_utf8(m_pSzTextWithoutColors[playerId], szText);
		FilterColors(m_pSzTextWithoutColors[playerId]);
		const char* pText = m_pSzTextWithoutColors[playerId];
		m_iOffset[playerId] = 0;
		while (*pText)
		{
			if (*pText == '\n')
			{
				m_iOffset[playerId]++;
			}
			pText++;
		}
		return;
	}
	m_dwColors[playerId] = dwColor;
	m_fDistance[playerId] = fDistance;
	m_dwTime[playerId] = dwTime;
	m_dwStartTime[playerId] = GetTickCount();
	m_bChatBubbleStatus[playerId] = 1;
	m_fTrueX[playerId] = -1.0f;
	m_pSzText[playerId] = new char[1024];
	m_pSzTextWithoutColors[playerId] = new char[1024];
	cp1251_to_utf8(m_pSzText[playerId], szText);
	cp1251_to_utf8(m_pSzTextWithoutColors[playerId], szText);
	FilterColors(m_pSzTextWithoutColors[playerId]);
	const char* pText = m_pSzTextWithoutColors[playerId];
	m_iOffset[playerId] = 0;
	while (*pText)
	{
		if (*pText == '\n')
		{
			m_iOffset[playerId]++;
		}
		pText++;
	}
}

void CPlayerTags::ResetChatBubble(PLAYERID playerId)
{
	if (m_bChatBubbleStatus[playerId])
	{
		m_dwTime[playerId] = 0;
	}
	m_bChatBubbleStatus[playerId] = 0;
}

void CPlayerTags::DrawChatBubble(PLAYERID playerId, CVector* vec, float fDistance)
{
	CVector TagPos;

	TagPos.x = vec->x;
	TagPos.y = vec->y;
	TagPos.z = vec->z;
	//TagPos.z = vec->Z;
	TagPos.z += 0.45f + (fDistance * 0.0675f) + ((float)m_iOffset[playerId] * pGUI->ScaleY(0.35f));

	CVector Out;
	// CSprite::CalcScreenCoors
	((void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA + 0x005C57E8 + 1))(&TagPos, &Out, nullptr, nullptr, false, false);

	if (Out.z < 1.0f)
		return;

	// name (id)
	ImVec2 pos = ImVec2(Out.x, Out.y);

	if (m_fTrueX[playerId] < 0)
	{
		char* curBegin = m_pSzTextWithoutColors[playerId];
		char* curPos = m_pSzTextWithoutColors[playerId];
		while (*curPos != '\0')
		{
			if (*curPos == '\n')
			{
				float width = ImGui::CalcTextSize(curBegin, (char*)(curPos - 1)).x;
				if (width > m_fTrueX[playerId])
				{
					m_fTrueX[playerId] = width;
				}

				curBegin = curPos + 1;
			}

			curPos++;
		}

		if (m_fTrueX[playerId] < 0)
		{
			m_fTrueX[playerId] = ImGui::CalcTextSize(m_pSzTextWithoutColors[playerId]).x;
		}
		//Log("m_fTrueX = %f", m_pTextLabels[x]->m_fTrueX);
	}

	pos.x -= (m_fTrueX[playerId] / 2);

	TextWithColors(pos, __builtin_bswap32(m_dwColors[playerId]), m_pSzText[playerId]);
}

void CPlayerTags::Draw(CVector* vec, char* szName, uint32_t dwColor,
	float fDist, float fHealth, float fArmour, bool bAfk, bool bVoice, bool bKeyboard)
{
	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_TAGS)) return;

	RwV3d TagPos;

	TagPos.x = vec->x;
	TagPos.y = vec->y;
	TagPos.z = vec->z;
	TagPos.z += 0.25f + (fDist * 0.0475f);

	CVector Out;
	// CSprite::CalcScreenCoors
	(( void (*)(RwV3d*, RwV3d*, float*, float*, bool, bool))(g_libGTASA+0x005C57E8+1))(&TagPos, &Out, nullptr, nullptr, false, false);

	if(Out.z < 1.0f)
		return;

	char tempBuff[300];
	cp1251_to_utf8(tempBuff, szName);

	// name (id)
	ImVec2 pos = ImVec2(Out.x, Out.y);
	pos.x -= ImGui::CalcTextSize(tempBuff).x/2;


	pGUI->RenderText(pos, __builtin_bswap32(dwColor | (0x000000FF)), true, tempBuff);

	// Health Bar
	if(fHealth < 0.0f) return;

	// округляем
//	Out.x = (float)((int)Out.x);
//	Out.y = (float)((int)Out.y);

	HealthBarColor = ImColor( 0xB9, 0x22, 0x28, 0xFF );
	HealthBarBGColor = ImColor( 0x4B, 0x0B, 0x14, 0xFF );

	float fWidth = pGUI->ScaleX( 60.0f );
	float fHeight = pGUI->ScaleY( 10.0f );
	float fOutline = (float)CSettings::m_Settings.iFontOutline;

	// top left
	HealthBarBDR1.x = Out.x - ((fWidth/2) + fOutline);
	HealthBarBDR1.y = Out.y + (pGUI->GetFontSize()*1.2f);//35.0f;
	// bottom right
	HealthBarBDR2.x = Out.x + ((fWidth/2) + fOutline);
	HealthBarBDR2.y = Out.y + (pGUI->GetFontSize()*1.2f) + fHeight;//48.0f;

	// top left
	HealthBarBG1.x = HealthBarBDR1.x + fOutline;//Out.x - 40.0f;
	HealthBarBG1.y = HealthBarBDR1.y + fOutline;//Out.y + 37.0f;
	// bottom right
	HealthBarBG2.x = HealthBarBDR2.x - fOutline;//Out.x + 40.0f;
	HealthBarBG2.y = HealthBarBDR2.y - fOutline;//Out.y + 46.0f;

	// top left
	HealthBar1.x = HealthBarBG1.x;//Out.x - 40.0f;
	HealthBar1.y = HealthBarBG1.y;//Out.y + 37.0f;
	// bottom right
	HealthBar2.y = HealthBarBG2.y;//Out.y + 46.0f;

	if (fHealth > 100.0f)
		fHealth = 100.0f;

	fHealth *= fWidth/100.0f;
	fHealth -= (fWidth/2);
	HealthBar2.x = Out.x + fHealth;

	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y += 13.0f;
		HealthBarBDR2.y += 13.0f;
		HealthBarBG1.y += 13.0f;
		HealthBarBG2.y += 13.0f;
		HealthBar1.y += 13.0f;
		HealthBar2.y += 13.0f;
	}

	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
	ImGui::GetOverlayDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);

	// Armour Bar
	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y -= 13.0f;
		HealthBarBDR2.y -= 13.0f;
		HealthBarBG1.y -= 13.0f;
		HealthBarBG2.y -= 13.0f;
		HealthBar1.y -= 13.0f;
		HealthBar2.y -= 13.0f;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if(fArmour > 100.0f)
			fArmour = 100.0f;

		fArmour *= fWidth/100.0f;
		fArmour -= (fWidth/2);
		HealthBar2.x = Out.x + fArmour;
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
		ImGui::GetOverlayDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);
	}

	// AFK Icon
	if(bAfk)
	{
		ImVec2 a = ImVec2(HealthBarBDR1.x - (pGUI->GetFontSize()*1.4f), HealthBarBDR1.y);
		ImVec2 b = ImVec2(a.x + (pGUI->GetFontSize()*1.3f), a.y + (pGUI->GetFontSize()*1.3f));
		ImGui::GetOverlayDrawList()->AddImage((ImTextureID)m_pAfk_icon->raster, a, b);
	}


}

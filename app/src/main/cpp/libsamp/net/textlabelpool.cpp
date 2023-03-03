#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "netgame.h"
#include "CSettings.h"

extern CNetGame *pNetGame;
extern CGUI *pGUI;

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors);

void CText3DLabelsPool::DrawNonAttached(TEXT_LABELS* pLabel)
{
	if (!pLabel)
	{
		return;
	}
	if (!pNetGame)
	{
		return;
	}
	if (!pNetGame->GetPlayerPool())
	{
		return;
	}
	CVector pos;
	memcpy((void*)&pos, (const void*)&(pLabel->pos), sizeof(CVector));
	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawAttachedToPlayer(TEXT_LABELS* pLabel)
{
	if (!pNetGame->GetPlayerPool()->GetLocalPlayer())
	{
		return;
	}
	CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	if (!pPed)
	{
		return;
	}

	if(!CSettings::m_Settings.iIsEnable3dTextInVehicle && pPed->IsInVehicle())
	{
		return;
	}
	if (!pLabel)
	{
		return;
	}
	CVector pos;
	memset((void*)&pos, 0, sizeof(CVector));

	PLAYERID playerId = pLabel->attachedToPlayerID;

	CPlayerPool* pPlayerPool = nullptr;
	if (pNetGame)
	{
		pPlayerPool = pNetGame->GetPlayerPool();
	}
	if (!pPlayerPool)
	{
		return;
	}

	if (!pPlayerPool->m_pPlayers[playerId])
	{
		return;
	}

	CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);
	if (!pPlayer)
	{
		return;
	}
	if (!pPlayer->GetPlayerPed())
	{
		return;
	}
	CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
	if (!pPlayerPed->IsAdded())
	{
		return;
	}
	CVector boneOut;
	pPlayerPed->GetBonePosition(5, boneOut);

	memcpy((void*)&pos, (const void*)&boneOut, sizeof(CVector));

	pos.x += pLabel->offsetCoords.x;
	pos.y += pLabel->offsetCoords.y;
	pos.z += pLabel->offsetCoords.z;

   // CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    float fDist = pPlayerPed->GetDistanceFromCamera();
    pos.z += (fDist * 0.055);

	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawAttachedToVehicle(TEXT_LABELS* pLabel)
{
	if (!pLabel)
	{
		return;
	}
	CVector pos;
	memset((void*)&pos, 0, sizeof(CVector));

	auto pVehicle = pNetGame->GetVehiclePool()->m_pVehicles[ pLabel->attachedToVehicleID ];

	if(pVehicle == nullptr || pVehicle->m_pVehicle == nullptr)
		return;

	MATRIX4X4 mat;
	memset((void*)& mat, 0, sizeof(MATRIX4X4));

	pVehicle->GetMatrix(&mat);

	memcpy((void*)& pos, (const void*)& mat.pos, sizeof(CVector));

	pos.x += pLabel->offsetCoords.x;
	pos.y += pLabel->offsetCoords.y;
	pos.z += pLabel->offsetCoords.z;

	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawVehiclesInfo()
{
	char inf[255];
	CVehiclePool* pVehiclePool = nullptr;
	if (pNetGame)
	{
		pVehiclePool = pNetGame->GetVehiclePool();
	}
	if (!pVehiclePool)
	{
		return;
	}
	for(int i = 0; i < MAX_VEHICLES; i++)
	{
		if(pVehiclePool->m_pVehicles[i])
		{
			int distance = pVehiclePool->m_pVehicles[i]->GetDistanceFromLocalPlayerPed();
			if(distance < 20){
				CVehicle* pVehicle = pVehiclePool->GetAt(i);

				CVector pos;
				memset((void*)&pos, 0, sizeof(CVector));

				MATRIX4X4 mat;
				memset((void*)& mat, 0, sizeof(MATRIX4X4));

				pVehicle->GetMatrix(&mat);

				memcpy((void*)& pos, (const void*)& mat.pos, sizeof(CVector));

				//memcpy((void*)& pos, (const void*)& mat.pos, sizeof(CVector));

				CVector Out;
				memset((void*)&Out, 0, sizeof(CVector));
				// CSprite::CalcScreenCoors
				((void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA + 0x005C57E8 + 1))(&pos, &Out, 0, 0, 0, 0);
				if (Out.z < 1.0f)
				{
					return;
				}

				ImVec2 vpos = ImVec2(Out.x, Out.y);


				sprintf(inf, "id: %d, type: %d\nHealth: %.2f\nDistance: %dm\n Immun: %d",
						i,
						pVehicle->GetModelIndex(),
						pVehicle->GetHealth(),
						distance,
						pVehicle->m_bIsInvulnerable
						);

				TextWithColors(vpos, __builtin_bswap32(0xf48fb1FF), inf, inf);
			}
		}

	}
}

void CText3DLabelsPool::DrawTextLabel(TEXT_LABELS* pLabel, CVector* pPos)
{
	if (!pNetGame->GetPlayerPool()->GetLocalPlayer())
	{
		return;
	}
	CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	if (!pPed)
	{
		return;
	}
	if (!pPed->IsAdded())
	{
		return;
	}

	if (pLabel->useLineOfSight)
	{
		CAMERA_AIM* pCam = GameGetInternalAim();
		if (!pCam)
		{
			return;
		}

		ScriptCommand(&get_line_of_sight,
			pPos->x, pPos->y, pPos->z,
			pCam->pos1x, pCam->pos1y, pCam->pos1z,
			1, 0, 0, 0, 0);
	}
	else
	{
		if (pPed->GetDistanceFromPoint(pPos->x, pPos->y, pPos->z) <= pLabel->drawDistance)
		{
			CVector Out;
			memset((void*)&Out, 0, sizeof(CVector));
			// CSprite::CalcScreenCoors
			((void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA + 0x005C57E8 + 1))(pPos, &Out, 0, 0, 0, 0);
			if (Out.z < 1.0f)
			{
				return;
			}

			ImVec2 pos = ImVec2(Out.x, Out.y);

			if (pLabel->m_fTrueX < 0.0f)
			{
				char* curBegin = pLabel->textWithoutColors;
				char* curPos = pLabel->textWithoutColors;
				while (*curPos != '\0')
				{
					if (*curPos == '\n')
					{
						float width = ImGui::CalcTextSize(curBegin, (char*)(curPos - 1)).x;
						if (width > pLabel->m_fTrueX)
						{
							pLabel->m_fTrueX = width;
						}

						curBegin = curPos + 1;
					}

					curPos++;
				}

				if (pLabel->m_fTrueX < 0.0f)
				{
					pLabel->m_fTrueX = ImGui::CalcTextSize(pLabel->textWithoutColors).x;
				}

			}

			//pos.x -= (pLabel->m_fTrueX / 2.0f);

			TextWithColors(pos, __builtin_bswap32(pLabel->color), pLabel->text, pLabel->textWithoutColors);
		}
	}
}

CText3DLabelsPool::CText3DLabelsPool()
{
	for(int i = 0; i < TEXT_LABEL_POOL_SIZE; i++)
	{
		m_pTextLabels[i] = nullptr;
		m_bSlotState[i] = false;
	}
}

CText3DLabelsPool::~CText3DLabelsPool()
{
	for (int i = 0; i < TEXT_LABEL_POOL_SIZE; i++)
	{
		if (m_pTextLabels[i] && m_bSlotState[i])
		{
			m_bSlotState[i] = false;

			m_pTextLabels[i]->text[0] = '\0';

			delete m_pTextLabels[i];
			m_pTextLabels[i] = nullptr;
		}
	}
}

void FilterColors(char* szStr)
{
	if(!szStr) return;

	char szNonColored[2048+1];
	int iNonColoredMsgLen = 0;

	for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szStr))
		{
			if(szStr[pos] == '{' && szStr[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColored[iNonColoredMsgLen] = szStr[pos];
		iNonColoredMsgLen++;
	}

	szNonColored[iNonColoredMsgLen] = 0;
	strcpy(szStr, szNonColored);
}

void CText3DLabelsPool::CreateTextLabel(int labelID, char* text, uint32_t color, 
	float posX, float posY, float posZ, float drawDistance, bool useLOS, PLAYERID attachedToPlayerID, VEHICLEID attachedToVehicleID)
{
	if (labelID < 0 || labelID >= TEXT_LABEL_POOL_SIZE)
	{
		return;
	}
	if (m_pTextLabels[labelID])
	{
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
		m_bSlotState[labelID] = false;
	}

	TEXT_LABELS* pTextLabel = new TEXT_LABELS;

	if (pTextLabel)
	{
		//pTextLabel->text = text;
		cp1251_to_utf8(pTextLabel->text, text);
		cp1251_to_utf8(pTextLabel->textWithoutColors, text);
		FilterColors(pTextLabel->textWithoutColors);

		pTextLabel->color = color;
		pTextLabel->pos.x = posX;
		pTextLabel->pos.y = posY;
		pTextLabel->pos.z = posZ;
		pTextLabel->drawDistance = drawDistance;
		pTextLabel->useLineOfSight = useLOS;
		pTextLabel->attachedToPlayerID = attachedToPlayerID;
		pTextLabel->attachedToVehicleID = attachedToVehicleID;

		pTextLabel->m_fTrueX = -1.0f;

		if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID)
		{
			pTextLabel->offsetCoords.x = posX;
			pTextLabel->offsetCoords.y = posY;
			pTextLabel->offsetCoords.z = posZ;
		}

		m_pTextLabels[labelID] = pTextLabel;
		m_bSlotState[labelID] = true;
	}
}

void CText3DLabelsPool::Delete(int labelID)
{
	if (labelID < 0 || labelID >= TEXT_LABEL_POOL_SIZE)
	{
		return;
	}
	m_bSlotState[labelID] = false;
	if (m_pTextLabels[labelID])
	{
		delete m_pTextLabels[labelID];
		m_pTextLabels[labelID] = nullptr;
	}
}

void CText3DLabelsPool::AttachToPlayer(int labelID, PLAYERID playerID, CVector pos)
{
	if (labelID < 0 || labelID >= TEXT_LABEL_POOL_SIZE)
	{
		return;
	}
	if (m_bSlotState[labelID] == true && m_pTextLabels[labelID])
	{
		//tempPlayerID = playerID;
		m_pTextLabels[labelID]->attachedToPlayerID = playerID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::AttachToVehicle(int labelID, VEHICLEID vehicleID, CVector pos)
{
	if (labelID < 0 || labelID >= TEXT_LABEL_POOL_SIZE)
	{
		return;
	}
	if (m_bSlotState[labelID] == true && m_pTextLabels[labelID])
	{
		m_pTextLabels[labelID]->attachedToVehicleID = vehicleID;
		m_pTextLabels[labelID]->pos = pos;
		m_pTextLabels[labelID]->offsetCoords = pos;
	}
}

void CText3DLabelsPool::Update3DLabel(int labelID, uint32_t color, char* text)
{
	if (labelID < 0 || labelID >= TEXT_LABEL_POOL_SIZE)
	{
		return;
	}
	Log("UPDATE LABEL");
	if (m_bSlotState[labelID] == true && m_pTextLabels[labelID])
	{
		m_pTextLabels[labelID]->color = color;
		//m_pTextLabels[labelID]->text = text;
		cp1251_to_utf8(m_pTextLabels[labelID]->text, text);
	}
}

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void TextWithColors(ImVec2 pos, ImColor col, const char* szStr, const char* szStrWithoutColors = nullptr)
{
	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
				if (pPlayer->GetPlayerPed())
				{
					if (pPlayer->GetPlayerPed()->GetActionTrigger() == ACTION_DEATH || pPlayer->GetPlayerPed()->IsDead())
					{
						return;
					}

					
				}
			}
		}
	}
	float fLineOffsets[100];

	int iCounter = 0;
	if (szStrWithoutColors)
	{
		const char* curBegin = szStrWithoutColors;
		const char* curPos = szStrWithoutColors;
		while (*curPos != '\0')
		{
			if (*curPos == '\n')
			{
				float width = ImGui::CalcTextSize(curBegin, (char*)(curPos - 1)).x;
				width /= 2.0f;

				fLineOffsets[iCounter] = width;
				iCounter++;

				curBegin = curPos + 1;
			}
			curPos++;
			if (*curPos == '\0')
			{
				float width = ImGui::CalcTextSize(curBegin, (char*)(curPos - 1)).x;
				width /= 2.0f;

				fLineOffsets[iCounter] = width;
				iCounter++;
			}
		}
	}
	
	iCounter = 0;
	char tempStr[4096];

	//va_list argPtr;
	//va_start(argPtr, fmt);
	//vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
	//va_end(argPtr);

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;

	if (szStrWithoutColors)
	{
		vecPos.x -= fLineOffsets[0];
	}

	//vecPos.x -= fLineOffsets[0];
	while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			// Print accumulated text
			if (textCur != textStart)
			{
				//ImGui::TextUnformatted(textStart, textCur);
				pGUI->RenderText(vecPos, col, true, textStart, textCur);
				//ImGui::SameLine(0.0f, 0.0f);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Process color code
			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			// Change color
			if (pushedColorStyle)
			{
				//ImGui::PopStyleColor();
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				//ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			// Print accumulated text an go to next line
			//ImGui::TextUnformatted(textStart, textCur);
			pGUI->RenderText(vecPos, col, true, textStart, textCur);
			iCounter++;

			vecPos.x = pos.x;//+= ImGui::CalcTextSize(textStart, textCur).x;
			if (szStrWithoutColors)
			{
				vecPos.x -= fLineOffsets[iCounter];
			}

			vecPos.y += pGUI->GetFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
		//ImGui::TextUnformatted(textStart, textCur);
	{
		pGUI->RenderText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
		//ImGui::NewLine();
		vecPos.y += pGUI->GetFontSize();

	//if(pushedColorStyle)
	//	ImGui::PopStyleColor();
}

#include "..//game/game.h"
extern CGame* pGame;
void CText3DLabelsPool::Draw()
{
	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_TEXTLABELS))
	{
		return;
	}
	if(pGame->m_bDl_enabled) DrawVehiclesInfo();
	for (int i = 0; i < TEXT_LABEL_POOL_SIZE; i++)
	{
		if (!m_pTextLabels[i] || !m_bSlotState[i])
		{
			continue;
		}
		TEXT_LABELS* pLabel = m_pTextLabels[i];
		if (!pNetGame->GetPlayerPool())
		{
			continue;
		}
		if (pLabel->attachedToPlayerID != INVALID_PLAYER_ID)
		{
			if (pLabel->attachedToPlayerID == pNetGame->GetPlayerPool()->GetLocalPlayerID())
			{
				continue;
			}
			DrawAttachedToPlayer(pLabel);
			continue;
		}
		else if (pLabel->attachedToVehicleID != INVALID_VEHICLE_ID)
		{
			DrawAttachedToVehicle(pLabel);
			continue;
		}
		else
		{
			DrawNonAttached(pLabel);
			continue;
		}

	}
}
bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
	const int hexCount = (int)(end - start);
	if (hexCount == 6 || hexCount == 8)
	{
		char hex[9];
		strncpy(hex, start, hexCount);
		hex[hexCount] = 0;

		unsigned int hexColor = 0;
		if (sscanf(hex, "%x", &hexColor) > 0)
		{
			color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
			color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
			color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
			color.w = 1.0f;

			if (hexCount == 8)
				color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;

			return true;
		}
	}

	return false;
}
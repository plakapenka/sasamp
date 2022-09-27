#include "../main.h"
#include "../gui/gui.h"
#include "../game/game.h"
#include "../chatwindow.h"
#include "netgame.h"

extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
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
	VECTOR pos;
	memcpy((void*)&pos, (const void*)&(pLabel->pos), sizeof(VECTOR));
	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawAttachedToPlayer(TEXT_LABELS* pLabel)
{
	if (!pLabel)
	{
		return;
	}
	VECTOR pos;
	memset((void*)&pos, 0, sizeof(VECTOR));

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

	if (!pPlayerPool->GetSlotState(playerId))
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
	VECTOR boneOut;
	pPlayerPed->GetBonePosition(5, &boneOut);

	memcpy((void*)&pos, (const void*)&boneOut, sizeof(VECTOR));

	pos.X += pLabel->offsetCoords.X;
	pos.Y += pLabel->offsetCoords.Y;
	pos.Z += pLabel->offsetCoords.Z;

	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawAttachedToVehicle(TEXT_LABELS* pLabel)
{
	if (!pLabel)
	{
		return;
	}
	VECTOR pos;
	memset((void*)&pos, 0, sizeof(VECTOR));

	VEHICLEID vehId = pLabel->attachedToVehicleID;

	CVehiclePool* pVehiclePool = nullptr;
	if (pNetGame)
	{
		pVehiclePool = pNetGame->GetVehiclePool();
	}
	if (!pVehiclePool)
	{
		return;
	}

	if (!pVehiclePool->GetSlotState(vehId))
	{
		return;
	}

	CVehicle* pVehicle = pVehiclePool->GetAt(vehId);
	if (!pVehicle)
	{
		return;
	}
	if (!pVehicle->IsAdded())
	{
		return;
	}
	MATRIX4X4 mat;
	memset((void*)& mat, 0, sizeof(MATRIX4X4));

	pVehicle->GetMatrix(&mat);

	memcpy((void*)& pos, (const void*)& mat.pos, sizeof(VECTOR));

	pos.X += pLabel->offsetCoords.X;
	pos.Y += pLabel->offsetCoords.Y;
	pos.Z += pLabel->offsetCoords.Z;

	DrawTextLabel(pLabel, &pos);
}

void CText3DLabelsPool::DrawTextLabel(TEXT_LABELS* pLabel, VECTOR* pPos)
{
	int hitEntity = 0;
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

		hitEntity = ScriptCommand(&get_line_of_sight,
			pPos->X, pPos->Y, pPos->Z,
			pCam->pos1x, pCam->pos1y, pCam->pos1z,
			1, 0, 0, 0, 0);
	}

	if (!pLabel->useLineOfSight || hitEntity)
	{
		if (pPed->GetDistanceFromPoint(pPos->X, pPos->Y, pPos->Z) <= pLabel->drawDistance)
		{
			VECTOR Out;
			memset((void*)&Out, 0, sizeof(VECTOR));
			// CSprite::CalcScreenCoors
			((void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_libGTASA + 0x54EEC0 + 1))(pPos, &Out, 0, 0, 0, 0);
			if (Out.Z < 1.0f)
			{
				return;
			}

			ImVec2 pos = ImVec2(Out.X, Out.Y);

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
		pTextLabel->pos.X = posX;
		pTextLabel->pos.Y = posY;
		pTextLabel->pos.Z = posZ;
		pTextLabel->drawDistance = drawDistance;
		pTextLabel->useLineOfSight = useLOS;
		pTextLabel->attachedToPlayerID = attachedToPlayerID;
		pTextLabel->attachedToVehicleID = attachedToVehicleID;

		pTextLabel->m_fTrueX = -1.0f;

		if (attachedToVehicleID != INVALID_VEHICLE_ID || attachedToPlayerID != INVALID_PLAYER_ID)
		{
			pTextLabel->offsetCoords.X = posX;
			pTextLabel->offsetCoords.Y = posY;
			pTextLabel->offsetCoords.Z = posZ;
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

void CText3DLabelsPool::AttachToPlayer(int labelID, PLAYERID playerID, VECTOR pos)
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

void CText3DLabelsPool::AttachToVehicle(int labelID, VEHICLEID vehicleID, VECTOR pos)
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
ImVec2 CalcTextSizeWithoutTags(char* szStr)
{
	if(!szStr) return ImVec2(0, 0);

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

	return ImGui::CalcTextSize(szNonColored);
}
void Render3DLabel(ImVec2 pos, char* utf8string, uint32_t dwColor)
{
	uint16_t linesCount = 0;
	std::string strUtf8 = utf8string;
	int size = strUtf8.length();
	std::string color;

	
	{	
		ImVec2 backgroundSize = CalcTextSizeWithoutTags(utf8string);
		backgroundSize.y += pGUI->GetFontSize() * 2;
		backgroundSize.x += pGUI->GetFontSize() * 2;

		ImVec2 backgroundPos1 = ImVec2(pos.x - (backgroundSize.x / 2), pos.y - (backgroundSize.y / 2));
		ImVec2 backgroundPos2 = ImVec2(backgroundPos1.x + backgroundSize.x, backgroundPos1.y + backgroundSize.y);

		ImGui::GetBackgroundDrawList()->AddRectFilled(backgroundPos1, backgroundPos2, ImColor(0.06f, 0.05f, 0.07f, 0.70f), 7.0f);
	}

	ALL:

	for(uint32_t i = 0; i < size; i++)
	{
		if(i+7 < strUtf8.length())
		{
			if(strUtf8[i] == '{' && strUtf8[i+7] == '}' )
			{
				color = strUtf8.substr(i, 7+1);
			}
		}
		if(strUtf8[i] == '\n')
		{
			linesCount++;
			if(i+1 < strUtf8.length() && !color.empty())
			{
				strUtf8.insert(i+1 , color);
				size += color.length();
				color.clear();
			}
		}
		if(strUtf8[i] == '\t')
		{
			strUtf8.replace(i, 1, " ");
		}
	}
	pos.y += pGUI->GetFontSize()*(linesCount / 2);
	if(linesCount)
	{
		uint16_t curLine = 0;
		uint16_t curIt = 0;
		for(uint32_t i = 0; i < strUtf8.length(); i++)
		{
			if(strUtf8[i] == '\n')
			{
				if(strUtf8[curIt] == '\n' )
				{
					curIt++;
				}
				ImVec2 _pos = pos;
				_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, i-curIt).c_str()).x / 2;
				_pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
				TextWithColors( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, i-curIt).c_str() );
				curIt = i;
				curLine++;
			}
		}
		if(strUtf8[curIt] == '\n')
		{
			curIt++;
		}
		if(strUtf8[curIt] != '\0')
		{
			ImVec2 _pos = pos;
			_pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, strUtf8.size()-curIt).c_str()).x / 2;
			_pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
			TextWithColors( _pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, strUtf8.length()-curIt).c_str() );
		}
	}
	else
	{
		pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.c_str()).x / 2;
		TextWithColors( pos, __builtin_bswap32(dwColor), (char*)strUtf8.c_str() );
	}

}

#include "..//game/game.h"
extern CGame* pGame;
void CText3DLabelsPool::Draw()
{
	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_TEXTLABELS))
	{
		return;
	}
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
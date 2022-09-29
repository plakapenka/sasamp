#include "main.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "CSettings.h"
#include "game/game.h"
#include "net/netgame.h"
#include "vendor/bass/bass.h"
#include <dlfcn.h>
#include <time.h>
#include "scrollbar.h"
#include "gui/CHUD.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;
extern CAMERA_AIM *pcaInternalAim;
extern CGame *pGame;
extern CChatWindow *pChatWindow;

#define NUM_OF_MESSAGES 100

template <typename T>
T GetBASSFunc(void *handle, const char *szFunc)
{
	return (T)dlsym(handle, szFunc);
}

BOOL(*BASS_Init_func)
(int device, DWORD freq, DWORD flags, void *win, void *dsguid);
HSTREAM(*BASS_StreamCreateURL_func)
(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
BOOL(*BASS_ChannelPlay_func)
(DWORD handlee, BOOL restart);
int (*BASS_ErrorGetCode_func)();
BOOL(*BASS_ChannelSet3DPosition_func)
(DWORD handlee, const BASS_3DVECTOR *pos, const BASS_3DVECTOR *orient, const BASS_3DVECTOR *vel);
BOOL(*BASS_StreamFree_func)
(HSTREAM handlee);
void (*BASS_Apply3D_func)();
BOOL(*BASS_Set3DPosition_func)
(const BASS_3DVECTOR *pos, const BASS_3DVECTOR *vel, const BASS_3DVECTOR *front, const BASS_3DVECTOR *top);
BOOL(*BASS_ChannelSet3DAttributes_func)
(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol);
BOOL(*BASS_ChannelSetAttribute_func)
(DWORD handle, DWORD attrib, float value);
DWORD(*BASS_ChannelFlags_func)
(DWORD handle, DWORD flags, DWORD mask);
BOOL(*BASS_ChannelStop_func)
(DWORD handle);
BOOL(*BASS_ChannelPause_func)
(DWORD handle);

BOOL BASS_Init(int device, DWORD freq, DWORD flags, void *win, void *dsguid)
{
	return BASS_Init_func(device, freq, flags, win, dsguid);
}

HSTREAM BASS_StreamCreateURL(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user)
{
	return BASS_StreamCreateURL_func(url, offset, flags, proc, user);
}

DWORD BASS_ChannelFlags(DWORD handle, DWORD flags, DWORD mask)
{
	return BASS_ChannelFlags_func(handle, flags, mask);
}

BOOL BASS_ChannelPlay(DWORD handlee, BOOL restart)
{
	return BASS_ChannelPlay_func(handlee, restart);
}

int BASS_ErrorGetCode()
{
	return BASS_ErrorGetCode_func();
}

BOOL BASS_ChannelSet3DPosition(DWORD handlee, const BASS_3DVECTOR *pos, const BASS_3DVECTOR *orient, const BASS_3DVECTOR *vel)
{
	return BASS_ChannelSet3DPosition_func(handlee, pos, orient, vel);
}

BOOL BASS_StreamFree(HSTREAM handlee)
{
	return BASS_StreamFree_func(handlee);
}

void BASS_Apply3D()
{
	return BASS_Apply3D_func();
}

BOOL BASS_Set3DPosition(const BASS_3DVECTOR *pos, const BASS_3DVECTOR *vel, const BASS_3DVECTOR *front, const BASS_3DVECTOR *top)
{
	return BASS_Set3DPosition_func(pos, vel, front, top);
}

BOOL BASS_ChannelSet3DAttributes(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol)
{
	return BASS_ChannelSet3DAttributes_func(handlee, mode, min, max, iangle, oangle, outvol);
}

BOOL BASS_ChannelSetAttribute(DWORD handle, DWORD attrib, float value)
{
	return BASS_ChannelSetAttribute_func(handle, attrib, value);
}

BOOL BASS_ChannelStop(DWORD handle)
{
	return BASS_ChannelStop_func(handle);
}

BOOL BASS_ChannelPause(DWORD handle)
{
	return BASS_ChannelPause_func(handle);
}

void InitBASSFuncs()
{
	void *handle;

#ifdef GAME_EDITION_CR
	handle = dlopen("/data/data/com.liverussia.cr/lib/libbass.so", 3);
#else
	handle = dlopen("/data/data/com.liverussia.cr/lib/libbass.so", 3);
#endif
	if (!handle)
	{
		Log("Cannot load libbass.so");
		return;
	}

	BASS_Init_func = GetBASSFunc<BOOL (*)(int device, DWORD freq, DWORD flags, void *win, void *)>(handle, "BASS_Init");

	BASS_StreamCreateURL_func = GetBASSFunc<HSTREAM (*)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user)>(handle, "BASS_StreamCreateURL");

	BASS_ChannelPlay_func = GetBASSFunc<BOOL (*)(DWORD handlee, BOOL restart)>(handle, "BASS_ChannelPlay");

	BASS_ErrorGetCode_func = GetBASSFunc<int (*)()>(handle, "BASS_ErrorGetCode");

	BASS_ChannelSet3DPosition_func = GetBASSFunc<BOOL (*)(DWORD handlee, const BASS_3DVECTOR *pos, const BASS_3DVECTOR *orient, const BASS_3DVECTOR *vel)>(handle, "BASS_ChannelSet3DPosition");

	BASS_StreamFree_func = GetBASSFunc<BOOL (*)(HSTREAM handlee)>(handle, "BASS_StreamFree");

	BASS_Apply3D_func = GetBASSFunc<void (*)()>(handle, "BASS_Apply3D");

	BASS_Set3DPosition_func = GetBASSFunc<BOOL (*)(const BASS_3DVECTOR *pos, const BASS_3DVECTOR *vel, const BASS_3DVECTOR *front, const BASS_3DVECTOR *top)>(handle, "BASS_Set3DPosition");

	BASS_ChannelSet3DAttributes_func = GetBASSFunc<BOOL (*)(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol)>(handle, "BASS_ChannelSet3DAttributes");

	BASS_ChannelSetAttribute_func = GetBASSFunc<BOOL (*)(DWORD handle, DWORD attrib, float value)>(handle, "BASS_ChannelSetAttribute");

	BASS_ChannelFlags_func = GetBASSFunc<DWORD (*)(DWORD handle, DWORD flags, DWORD mask)>(handle, "BASS_ChannelFlags");

	BASS_ChannelPause_func = GetBASSFunc<BOOL (*)(DWORD handle)>(handle, "BASS_ChannelPause");

	BASS_ChannelStop_func = GetBASSFunc<BOOL (*)(DWORD handle)>(handle, "BASS_ChannelStop");
}

BOOL returnedValue;
#include "util/CJavaWrapper.h"

#ifdef GAME_EDITION_CR
uint32_t g_uiHeadMoveEnabled = 0;
#else
uint32_t g_uiHeadMoveEnabled = 1;
#endif

uint32_t g_uiBorderedText = 1;
#include "CDebugInfo.h"
#include "CLocalisation.h"
#include "scoreboard.h"
#include "game/CCustomPlateManager.h"
#include <fcntl.h>

extern CScoreBoard *pScoreBoard;
bool ProcessVoiceCommands(const char *str)
{
	if (strcmp(str, "/q") == 0)
	{
		// Update this flag so DoGameRestart finishes the game.
		*(uint8_t *)(g_libGTASA + 0x63E094) = 0;

		// DoGameRestart
		((void (*)())(g_libGTASA + 0x261C8C + 1))();
		return true;
	}

	if (strstr(str, "/reconnect"))
	{
		pNetGame->ShutDownForGameRestart();
		pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
		return true;
	}

	if (strstr(str, "/chat"))
	{
		if (pGame->IsToggledHUDElement(0))
		{
			pGame->ToggleHUDElement(0, false);
		}
		else
		{
			pGame->ToggleHUDElement(0, true);
		}
		return true;
	}

	if (strstr(str, "/gui"))
	{
		if (pGame->IsToggledHUDElement(0))
		{
			for (int i = 0; i < 7; i++)
			{
				pGame->ToggleHUDElement(i, false);
			}
			pGame->ToggleHUDElement(1, false);
			g_pJavaWrapper->HideKilllist();
			g_pJavaWrapper->HideHud();
			g_pJavaWrapper->HideVoice();
			g_pJavaWrapper->HideSamwill();
			g_pJavaWrapper->HideSamwillMoney();
		}
		else
		{
			for (int i = 0; i < 7; i++)
			{
				pGame->ToggleHUDElement(i, true);
			}
			pGame->ToggleHUDElement(1, true);
			g_pJavaWrapper->ShowHud();
			g_pJavaWrapper->ShowVoice();
		}
		return true;
	}

	if (strstr(str, "/pagesize"))
	{
		while (*str)
		{
			if (*str == ' ')
			{
				str++;
				break;
			}
			str++;
		}
		int lines = 0;
		if (sscanf(str, "%d", &lines) == -1)
		{
			pChatWindow->AddDebugMessage("Используйте: /pagesize [Кол-во строк].");
			return true;
		}
		if (lines < 4 || lines > 12)
		{
			pChatWindow->AddDebugMessage("Количество строк в чате не может быть меньше 4 и больше 12.");
			return true;
		}
		pSettings->GetWrite().iChatMaxMessages = lines - 1;
		pChatWindow->AddDebugMessage("Количество строк в чате изменено на %d.", lines);
		pChatWindow->m_bPendingReInit = true;
		return true;
	}

	if (strstr(str, "/fontsize"))
	{
		while (*str)
		{
			if (*str == ' ')
			{
				str++;
				break;
			}
			str++;
		}
		int fsize = 0;
		if (sscanf(str, "%d", &fsize) == -1)
		{
			pChatWindow->AddDebugMessage("Используйте: /fontsize [Размер шрифта].");
			return true;
		}
		if (fsize < 1 || fsize > 50)
		{
			pChatWindow->AddDebugMessage("Размер шрифта не может быть меньше 1 и больше 50.");
			return true;
		}
		pSettings->GetWrite().fFontSize = (float)fsize;
		pChatWindow->AddDebugMessage("Размер шрифта изменён на %d.", fsize);
		pChatWindow->m_bPendingReInit = true;
		return true;
	}

	if (strstr(str, "/headmove"))
	{
		if (pSettings->GetReadOnly().szHeadMove == 1)
		{
			pChatWindow->AddDebugMessage("Вращение головой {ff0000}выключено");
			pSettings->GetWrite().szHeadMove = 0;
		}
		else
		{
			pChatWindow->AddDebugMessage("Вращение головой {00ff00}включено");
			pSettings->GetWrite().szHeadMove = 1;
		}
		return true;
	}

	if (strstr(str, "/debug"))
	{
		if (pSettings->GetReadOnly().szDebug == 1)
		{
			pChatWindow->AddDebugMessage("Debug Mode: {ff0000}disabled");
			pSettings->GetWrite().szDebug = 0;
		}
		else
		{
			pChatWindow->AddDebugMessage("Debug Mode: {00ff00}enabled");
			pSettings->GetWrite().szDebug = 1;
		}
		return true;
	}

	if (strstr(str, "/dl"))
	{
		if (pSettings->GetReadOnly().szDL == 1)
		{
			pChatWindow->AddDebugMessage("Отображение информации об автомобилях {ff0000}выключено");
			pSettings->GetWrite().szDL = 0;
		}
		else
		{
			pChatWindow->AddDebugMessage("Отображение информации об автомобилях {00ff00}включено");
			pSettings->GetWrite().szDL = 1;
		}

		return true;
	}

	if (strstr(str, "/timestamp"))
	{
		if (pSettings->GetReadOnly().szTimeStamp == 1)
		{
			pChatWindow->AddDebugMessage("Отображение времени в чате {ff0000}выключено");
			pSettings->GetWrite().szTimeStamp = 0;
		}
		else
		{
			pChatWindow->AddDebugMessage("Отображение времени в чате {00ff00}включено");
			pSettings->GetWrite().szTimeStamp = 1;
		}

		return true;
	}

	if (strstr(str, "/tab"))
	{
		pScoreBoard->Toggle();
		return true;
	}

	if (strstr(str, "/settings"))
	{
		g_pJavaWrapper->ShowClientSettings();
		return true;
	}
	return false;
}

void ChatWindowInputHandler(const char *str)
{
	if (!str || *str == '\0')
		return;
	if (!pNetGame)
		return;

	auto pCmd = pChatWindow->bufferedChat.WriteLock();

	if (*str == '/')
	{
		pCmd->type = 0;
	}
	else
	{
		pCmd->type = 1;
	}

	ProcessVoiceCommands(str);

	strcpy(pCmd->buff, str);

	pChatWindow->bufferedChat.WriteUnlock();

	return;
}

CChatWindow::CChatWindow()
{
	m_uiMaxTimeChatHide = 0;
	m_uiTimeChatHideBegin = 0;
	pScrollbar = nullptr;

	m_uiLastTimePushedMessage = GetTickCount();
	m_bNewMessage = false;
	m_uiTimePushed = GetTickCount();
	m_bPendingReInit = false;
	ReInit();
}

CChatWindow::~CChatWindow()
{
	if (pScrollbar)
	{
		delete pScrollbar;
		pScrollbar = nullptr;
	}
}

#include <mutex>
static std::mutex lDebugMutex;

#include "dialog.h"
extern CDialogWindow *pDialogWindow;

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{

	static bool bWannaOpenChat = false;

	switch (type)
	{
	case TOUCH_PUSH:
		if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
			y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
		{
			bWannaOpenChat = true;
		}
		break;

	case TOUCH_POP:
		if (bWannaOpenChat &&
			x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
			y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
		{
			if (pDialogWindow)
			{
				if (pDialogWindow->m_bRendered)
					return true;
			}
			if (pKeyBoard)
			{
				pKeyBoard->Open(&ChatWindowInputHandler);
			}
			m_uiLastTimePushedMessage = GetTickCount();
		}
		bWannaOpenChat = false;
		break;

	case TOUCH_MOVE:
		break;
	}

	return true;
}

void CChatWindow::Render()
{
	if (m_bPendingReInit)
	{
		ReInit();
		m_bPendingReInit = false;
	}

	if (!pGame->IsToggledHUDElement(HUD_ELEMENT_CHAT))
		return;
	if (pScrollbar)
	{
		pScrollbar->m_bDrawState = false;
		if (pKeyBoard && pDialogWindow)
		{
			if (pKeyBoard->IsOpen() && !pDialogWindow->m_bRendered)
			{
				pScrollbar->m_bDrawState = true;
			}
		}
		pScrollbar->Draw();
	}
	if (false)
	{
		ImGui::GetBackgroundDrawList()->AddRect(
			ImVec2(m_fChatPosX, m_fChatPosY),
			ImVec2(m_fChatPosX + m_fChatSizeX, m_fChatPosY + m_fChatSizeY),
			IM_COL32_BLACK);
	}

	// ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
	if (!pScrollbar)
	{
		if (pScrollbar->GetValue() < 0 || pScrollbar->GetValue() > NUM_OF_MESSAGES + m_iMaxMessages)
		{
			return;
		}
		return;
	}

	if (pKeyBoard)
	{
		if (pKeyBoard->IsOpen())
		{
			m_uiLastTimePushedMessage = GetTickCount();
		}
	}

	uint32_t currentOffset = 1;

	bool firstMessageAppear = false;

	/*
	ImGui::Begin("", nullptr , ImVec2( m_fChatSizeX, m_fChatSizeY + pGUI->GetFontSize() ) , 0.9f,
			ImGuiWindowFlags_NoCollapse | 
			ImGuiWindowFlags_NoTitleBar | 
			ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoScrollbar );

		pGUI->m_imWindowChat = ImGui::GetCurrentWindow();

		for (int i = 100 - pScrollbar->GetValue(); i < 100 - pScrollbar->GetValue() + m_iMaxMessages + 1 + 9999; i++) // 9999 сообщений это максимальное количество сообщений в чате
		{
			if (i >= m_vChatWindowEntries.size())
			{
				break;
			}
			if (!m_vChatWindowEntries[i])
			{
				break;
			}

			ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
			float fProgressedAlpha = 1.0f;
			float fY_ = pGUI->GetFontSize() * (float)currentOffset;
			if (m_bNewMessage)
			{
				uint32_t timeSpent = GetTickCount() - m_uiTimePushed;
				if (timeSpent >= NEW_MESSAGE_PUSH_TIME)
				{
					m_bNewMessage = false;
					m_uiTimePushed = 0;
				}
				else
				{
					float fProgress = (float)timeSpent / (float)NEW_MESSAGE_PUSH_TIME;
					fProgressedAlpha = fProgress;
					fY_ = ((pGUI->GetFontSize() * fProgress) + fY_ - pGUI->GetFontSize());
				}
			}
			pos.y -= fY_;
			currentOffset++;
			if (firstMessageAppear)
			{
				fProgressedAlpha = 1.0f;
			}
			if (!firstMessageAppear)
			{
				firstMessageAppear = true;
			}

			auto entry = m_vChatWindowEntries[i];

			char chattextbuf[450];
			if (pSettings->GetReadOnly().szTimeStamp == 1)
			{
				sprintf(chattextbuf, "%s %s", entry->szTimeStamp, entry->utf8Message);
			}
			else
			{
				strcpy(chattextbuf, entry->utf8Message);
			}

			switch (entry->eType)
			{
			case CHAT_TYPE_CHAT:
				//if (entry->szNick[0] != 0)
				//{
				//	ImGui::Text("%s", entry->szNick);
				//	pos.x += ImGui::CalcTextSize(entry->szNick).x + ImGui::CalcTextSize(" ").x; //+ pGUI->GetFontSize() * 0.4;
				//}
				ImGui::Text("%s", chattextbuf);
				break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				ImGui::Text("%s", chattextbuf);
				break;
			}

			pos.x = m_fChatPosX;
			pos.y -= pGUI->GetFontSize();
		}

	ImGui::SetWindowPos(ImVec2(m_fChatPosX, m_fChatPosY - pGUI->GetFontSize()));
	ImGui::End();
	*/


	for (int i = 100 - pScrollbar->GetValue(); i < 100 - pScrollbar->GetValue() + m_iMaxMessages + 1; i++)
	{
		if (i >= m_vChatWindowEntries.size())
		{
			break;
		}
		if (!m_vChatWindowEntries[i])
		{
			break;
		}

		ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY + m_fChatSizeY);
		float fProgressedAlpha = 1.0f;
		float fY_ = pGUI->GetFontSize() * (float)currentOffset;
		if (m_bNewMessage)
		{
			uint32_t timeSpent = GetTickCount() - m_uiTimePushed;
			if (timeSpent >= NEW_MESSAGE_PUSH_TIME)
			{
				m_bNewMessage = false;
				m_uiTimePushed = 0;
			}
			else
			{
				float fProgress = (float)timeSpent / (float)NEW_MESSAGE_PUSH_TIME;
				fProgressedAlpha = fProgress;
				fY_ = ((pGUI->GetFontSize() * fProgress) + fY_ - pGUI->GetFontSize());
			}
		}
		pos.y -= fY_;
		currentOffset++;
		if (firstMessageAppear)
		{
			fProgressedAlpha = 1.0f;
		}
		if (!firstMessageAppear)
		{
			firstMessageAppear = true;
		}

		auto entry = m_vChatWindowEntries[i];

		char chattextbuf[450];
		if (pSettings->GetReadOnly().szTimeStamp == 1)
		{
			sprintf(chattextbuf, "%s %s", entry->szTimeStamp, entry->utf8Message);
		}
		else
		{
			strcpy(chattextbuf, entry->utf8Message);
		}

		switch (entry->eType)
		{
		case CHAT_TYPE_CHAT:
			//if (entry->szNick[0] != 0)
			//{
			//	RenderText(entry->szNick, pos.x, pos.y, entry->dwNickColor, fProgressedAlpha);
			//	pos.x += ImGui::CalcTextSize(entry->szNick).x + ImGui::CalcTextSize(" ").x; //+ pGUI->GetFontSize() * 0.4;
			//}
			RenderText(chattextbuf, pos.x, pos.y, entry->dwTextColor, fProgressedAlpha);
			break;

		case CHAT_TYPE_INFO:
		case CHAT_TYPE_DEBUG:
			RenderText(chattextbuf, pos.x, pos.y, entry->dwTextColor, fProgressedAlpha);
			break;
		}

		pos.x = m_fChatPosX;
		pos.y -= pGUI->GetFontSize();
	}
}

bool ProcessInlineHexColor(const char *start, const char *end, ImVec4 &color)
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

void CChatWindow::ReInit()
{
	m_fChatPosX = pGUI->ScaleX(pSettings->GetReadOnly().fChatPosX);
	m_fChatPosY = pGUI->ScaleY(pSettings->GetReadOnly().fChatPosY);
	m_fChatSizeX = pGUI->ScaleX(pSettings->GetReadOnly().fChatSizeX);
	m_fChatSizeY = pGUI->ScaleY(pSettings->GetReadOnly().fChatSizeY);
	m_iMaxMessages = pSettings->GetReadOnly().iChatMaxMessages;

	m_fChatSizeY = m_iMaxMessages * pGUI->GetFontSize();

	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xBEBEBEFF;

	if (!pScrollbar)
	{
		pScrollbar = new CScrollbar();
	}
	pScrollbar->m_fX = m_fChatPosX - pGUI->ScaleX(15.0f);
	pScrollbar->m_fY = m_fChatPosY - pGUI->ScaleY(10.0f);

	pScrollbar->m_fWidthBox = pGUI->ScaleX(7.0f);
	pScrollbar->m_fHeightBox = pGUI->ScaleY(80.0f);

	pScrollbar->m_fUpY = m_fChatPosY - pGUI->ScaleY(20.0f);
	pScrollbar->m_fDownY = m_fChatSizeY + m_fChatPosY + pGUI->ScaleY(5.0f);

	pScrollbar->m_iMaxValue = NUM_OF_MESSAGES;
	pScrollbar->SetOnPos(NUM_OF_MESSAGES);

	m_uiLastTimePushedMessage = GetTickCount();

	m_fPosBeforeBoundChat = m_fChatPosY;
	m_fOffsetBefore = 0.0f;
}

void CChatWindow::RenderText(const char *u8Str, float posX, float posY, uint32_t dwColor, float alphaNewMessage)
{
	const char *textStart = u8Str;
	const char *textCur = u8Str;
	const char *textEnd = u8Str + strlen(u8Str);

	uint8_t bAlpha = GetAlphaFromLastTimePushedMessage();

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	if (bAlpha != 255)
	{
		colorCur.Value.w = (float)bAlpha / 255.0f;
	}
	ImVec4 col;

	if (alphaNewMessage != 1.0f)
	{
		colorCur.Value.w = alphaNewMessage;
	}

	while (*textCur)
	{
		if (textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			if (textCur != textStart)
			{
				pGUI->RenderTextWithSize(posCur, colorCur, true, textStart, textCur, pSettings->GetReadOnly().fFontSize);

				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			if (ProcessInlineHexColor(textCur + 1, textCur + 7, col))
			{
				colorCur = col;
				if (bAlpha != 255)
				{
					colorCur.Value.w = (float)bAlpha / 255.0f;
				}
				if (alphaNewMessage != 1.0f)
				{
					colorCur.Value.w = alphaNewMessage;
				}
			}

			// Aaeaaai niauaiea
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if (textCur != textStart)
		pGUI->RenderTextWithSize(posCur, colorCur, true, textStart, textCur, pSettings->GetReadOnly().fFontSize);

	return;
}

void CChatWindow::SetChatDissappearTimeout(uint32_t uiTimeoutStart, uint32_t uiTimeoutEnd)
{
	m_uiMaxTimeChatHide = uiTimeoutEnd;
	m_uiTimeChatHideBegin = uiTimeoutStart;
}

void CChatWindow::ProcessPushedCommands()
{
	BUFFERED_COMMAND_CHAT *pCmd = nullptr;
	while (pCmd = bufferedChat.ReadLock())
	{
		if (pCmd->buff[0] == '/')
		{
			pNetGame->SendChatCommand(pCmd->buff);
		}
		else
		{
			pNetGame->SendChatMessage(pCmd->buff);
		}

		bufferedChat.ReadUnlock();
	}
}

void CChatWindow::AddChatMessage(char *szNick, uint32_t dwNickColor, char *szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddDebugMessageNonFormatted(char *szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, szStr, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char *szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::SetLowerBound(int bound)
{
	ImGuiIO &io = ImGui::GetIO();

	m_fChatPosY = m_fPosBeforeBoundChat;
	pScrollbar->m_fY += m_fOffsetBefore;
	pScrollbar->m_fDownY += m_fOffsetBefore;

	float y = io.DisplaySize.y - (float)bound;

	m_fOffsetBefore = 0.0f;

	while (m_fChatPosY + m_fChatSizeY + pGUI->GetFontSize() * 2.0f >= y)
	{
		m_fChatPosY -= pGUI->GetFontSize();
		m_fOffsetBefore += pGUI->GetFontSize();
	}

	pScrollbar->m_fY -= m_fOffsetBefore;
	pScrollbar->m_fDownY -= m_fOffsetBefore;
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY &entry)
{
	if (m_vChatWindowEntries.size() >= NUM_OF_MESSAGES + m_iMaxMessages)
	{
		auto it = m_vChatWindowEntries.back();
		delete it;
		m_vChatWindowEntries.pop_back();
	}

	CHAT_WINDOW_ENTRY *pEntry = new CHAT_WINDOW_ENTRY;
	memcpy(pEntry, &entry, sizeof(CHAT_WINDOW_ENTRY));

	m_bNewMessage = true;
	m_uiTimePushed = GetTickCount();

	m_vChatWindowEntries.insert(std::begin(m_vChatWindowEntries), pEntry);
	return;
}

uint8_t CChatWindow::GetAlphaFromLastTimePushedMessage()
{
	if (!m_uiMaxTimeChatHide || !m_uiTimeChatHideBegin)
	{
		return 0xFF;
	}

	if ((GetTickCount() - m_uiLastTimePushedMessage) >= TIME_CHAT_HIDE_BEGIN)
	{
		uint32_t time = GetTickCount() - m_uiLastTimePushedMessage - TIME_CHAT_HIDE_BEGIN;
		uint32_t distance = MAX_TIME_CHAT_HIDE - TIME_CHAT_HIDE_BEGIN;
		if (!distance)
		{
			return 0xFF;
		}
		float res = (float)time / (float)distance;
		if (res >= 1.0f)
		{
			return 0;
		}
		return (255 - (uint8_t)(res * 255.0f));
	}
	return 0xFF;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char *szString, char *szNick, uint32_t dwTextColor, uint32_t dwNickColor)
{
	m_uiLastTimePushedMessage = GetTickCount();

	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if (szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else
		entry.szNick[0] = '\0';

	time_t     now = time(0);
    struct tm  tstruct;
    tstruct = *localtime(&now);
    strftime(entry.szTimeStamp, sizeof(entry.szTimeStamp), "%[%X%]", &tstruct);

	if (type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		while (szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		if ((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString + MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString + (iBestLineLength + 1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}

	return;
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while (*szString)
	{
		if (*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}
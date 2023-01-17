#include "main.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "CSettings.h"
#include "game/game.h"
#include "net/netgame.h"
#include "vendor/bass/bass.h"
#include <dlfcn.h>
#include "java_systems/CTab.h"
#include "java_systems/CHUD.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;
extern CAMERA_AIM * pcaInternalAim;
extern CGame * pGame;
int CChatWindow::cursorStart = 0;
int CChatWindow::cursorEnd = 0;
bool g_bShowVoiceList = false;

#define NUM_OF_MESSAGES	100

template<typename T> T GetBASSFunc(void* handle, const char* szFunc)
{
	return (T)dlsym(handle, szFunc);
}

BOOL(*BASS_Init_func)(int device, DWORD freq, DWORD flags, void* win, void* dsguid);
HSTREAM(*BASS_StreamCreateURL_func)(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC* proc, void* user);
BOOL(*BASS_ChannelPlay_func)(DWORD handlee, BOOL restart);
int (*BASS_ErrorGetCode_func)();
BOOL(*BASS_ChannelSet3DPosition_func)(DWORD handlee, const BASS_3DVECTOR* pos, const BASS_3DVECTOR* orient, const BASS_3DVECTOR* vel);
BOOL(*BASS_StreamFree_func)(HSTREAM handlee);
BOOL(*BASS_Set3DFactors_func)(float distf, float rollf, float doppf);
void (*BASS_Apply3D_func)();
BOOL(*BASS_Set3DPosition_func)(const BASS_3DVECTOR* pos, const BASS_3DVECTOR* vel, const BASS_3DVECTOR* front, const BASS_3DVECTOR* top);
BOOL(*BASS_ChannelSet3DAttributes_func)(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol);
BOOL(*BASS_ChannelSetAttribute_func)(DWORD handle, DWORD attrib, float value);
DWORD (*BASS_ChannelFlags_func)(DWORD handle, DWORD flags, DWORD mask);


BOOL BASS_Init(int device, DWORD freq, DWORD flags, void* win, void* dsguid)
{
	return BASS_Init_func(device, freq, flags, win, dsguid);
}

HSTREAM BASS_StreamCreateURL(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC* proc, void* user)
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

BOOL BASS_ChannelSet3DPosition(DWORD handlee, const BASS_3DVECTOR* pos, const BASS_3DVECTOR* orient, const BASS_3DVECTOR* vel)
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

BOOL BASS_Set3DFactors(float distf, float rollf, float doppf)
{
	return BASS_Set3DFactors_func(distf, rollf, doppf);
}

BOOL BASS_Set3DPosition(const BASS_3DVECTOR* pos, const BASS_3DVECTOR* vel, const BASS_3DVECTOR* front, const BASS_3DVECTOR* top)
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

void InitBASSFuncs()
{
	void* handle;

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

	BASS_Init_func = GetBASSFunc<BOOL(*)(int device, DWORD freq, DWORD flags, void* win, void*)>(handle, "BASS_Init");

	BASS_StreamCreateURL_func = GetBASSFunc<HSTREAM(*)(const char* url, DWORD offset, DWORD flags, DOWNLOADPROC * proc, void* user)>(handle, "BASS_StreamCreateURL");

	BASS_ChannelPlay_func = GetBASSFunc<BOOL(*)(DWORD handlee, BOOL restart)>(handle, "BASS_ChannelPlay");

	BASS_ErrorGetCode_func = GetBASSFunc<int (*)() >(handle, "BASS_ErrorGetCode");

	BASS_ChannelSet3DPosition_func = GetBASSFunc< BOOL(*)(DWORD handlee, const BASS_3DVECTOR * pos, const BASS_3DVECTOR * orient, const BASS_3DVECTOR * vel)>(handle, "BASS_ChannelSet3DPosition");

	BASS_StreamFree_func = GetBASSFunc<BOOL(*)(HSTREAM handlee)>(handle, "BASS_StreamFree");

	BASS_Apply3D_func = GetBASSFunc<void (*)()>(handle, "BASS_Apply3D");

	BASS_Set3DFactors_func = GetBASSFunc<BOOL(*)(float distf, float rollf, float doppf) >(handle, "BASS_Set3DFactors");

	BASS_Set3DPosition_func = GetBASSFunc<BOOL(*)(const BASS_3DVECTOR * pos, const BASS_3DVECTOR * vel, const BASS_3DVECTOR * front, const BASS_3DVECTOR * top)>(handle, "BASS_Set3DPosition");

	BASS_ChannelSet3DAttributes_func = GetBASSFunc<BOOL(*)(DWORD handlee, int mode, float min, float max, int iangle, int oangle, float outvol)>(handle, "BASS_ChannelSet3DAttributes");

	BASS_ChannelSetAttribute_func = GetBASSFunc<BOOL(*)(DWORD handle, DWORD attrib, float value)>(handle, "BASS_ChannelSetAttribute");

	BASS_ChannelFlags_func = GetBASSFunc< DWORD(*)(DWORD handle, DWORD flags, DWORD mask)>(handle, "BASS_ChannelFlags");
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
#include "game/CCustomPlateManager.h"
#include "java_systems/inventrory.h"
#include <fcntl.h>


CChatWindow::CChatWindow()
{

}

CChatWindow::~CChatWindow()
{

}

#include <mutex>
static std::mutex lDebugMutex;

extern CINVENTORY *pInventory;

void CChatWindow::ProcessPushedCommands()
{
	BUFFERED_COMMAND_CHAT* pCmd = nullptr;
//	while (pCmd = bufferedChat.ReadLock())
//	{
//		if (pCmd->buff[0] == '/')
//		{
//			pNetGame->SendChatCommand(pCmd->buff);
//		}
//		else
//		{
//			pNetGame->SendChatMessage(pCmd->buff);
//		}
//
//		//bufferedChat.ReadUnlock();
//	}
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	CHUD::AddChatMessage(szMessage);
	//AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	CHUD::AddChatMessage(tmp_buf);
	//AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

void CChatWindow::AddDebugMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);

	CHUD::AddChatMessage(tmp_buf);
	//AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::AddDebugMessageNonFormatted(char* szStr)
{
	FilterInvalidChars(szStr);
	CHUD::AddChatMessage(szStr);
	//AddToChatWindowBuffer(CHAT_TYPE_DEBUG, szStr, nullptr, m_dwDebugColor, 0);
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}

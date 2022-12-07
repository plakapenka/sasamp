#include "main.h"
#include "CSettings.h"
#include "game/game.h"
#include "vendor/ini/config.h"
#include "game/CAdjustableHudColors.h"
#include "game/CAdjustableHudPosition.h"
#include "game/CAdjustableHudScale.h"
#include "java_systems/CHUD.h"
extern CGame* pGame;
extern CHUD *pHud;

static void ClearBackslashN(char *pStr, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		if (pStr[i] == '\n' || pStr[i] == 13)
		{
			pStr[i] = 0;
		}
	}
}

CSettings::CSettings()
{
	LoadSettings(nullptr);
}

CSettings::~CSettings()
{
}

void CSettings::ToDefaults(int iCategory)
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);

	FILE *pFile = fopen(buff, "w");

	fwrite("[gui]", 1, 6, pFile);

	fclose(pFile);

	Save(iCategory);
	LoadSettings(m_Settings.szNickName);

	pHud->ChangeChatHeight(m_Settings.iChatMaxMessages);
	pHud->ChangeChatTextSize(m_Settings.iChatFontSize);

}

void CSettings::Save(int iIgnoreCategory)
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);
	remove(buff);

	ini_table_s *config = ini_table_create();

	ini_table_create_entry(config, "client", "name", m_Settings.szNickName);
	ini_table_create_entry(config, "client", "password", m_Settings.szPassword);
	ini_table_create_entry(config, "client", "player_password", m_Settings.player_password);
	
	ini_table_create_entry_as_int(config, "client", "autologin", m_Settings.szAutoLogin);
	ini_table_create_entry_as_int(config, "gui", "hparmourtext", m_Settings.iHPArmourText);
	


	ini_table_create_entry_as_int(config, "client", "server", m_Settings.szServer);
	ini_table_create_entry_as_int(config, "client", "debug", m_Settings.szDebug);
	ini_table_create_entry_as_int(config, "client", "headmove", m_Settings.szHeadMove);
	ini_table_create_entry_as_int(config, "client", "dl", m_Settings.szDL);
	ini_table_create_entry_as_int(config, "client", "timestamp", m_Settings.szTimeStamp);

	ini_table_create_entry(config, "gui", "Font", m_Settings.szFont);

	ini_table_create_entry_as_float(config, "gui", "FontSize", m_Settings.fFontSize);
	ini_table_create_entry_as_int(config, "gui", "FontOutline", m_Settings.iFontOutline);

	if (iIgnoreCategory != 2)
	{

		ini_table_create_entry_as_float(config, "gui", "MicrophoneSize", m_Settings.fButtonMicrophoneSize);
		ini_table_create_entry_as_float(config, "gui", "MicrophoneX", m_Settings.fButtonMicrophoneX);
		ini_table_create_entry_as_float(config, "gui", "MicrophoneY", m_Settings.fButtonMicrophoneY);

		ini_table_create_entry_as_float(config, "gui", "ButtonEnterPassengerSize", m_Settings.fButtonEnterPassengerSize);
		ini_table_create_entry_as_float(config, "gui", "ButtonEnterPassengerX", m_Settings.fButtonEnterPassengerX);
		ini_table_create_entry_as_float(config, "gui", "ButtonEnterPassengerY", m_Settings.fButtonEnterPassengerY);

		ini_table_create_entry_as_float(config, "gui", "CameraCycleSize", m_Settings.fButtonCameraCycleSize);
		ini_table_create_entry_as_float(config, "gui", "CameraCycleX", m_Settings.fButtonCameraCycleX);
		ini_table_create_entry_as_float(config, "gui", "CameraCycleY", m_Settings.fButtonCameraCycleY);
	}

	ini_table_create_entry_as_int(config, "gui", "fps", m_Settings.iFPS);

	if (iIgnoreCategory != 1)
	{
		ini_table_create_entry_as_int(config, "gui", "ChatFontSize", m_Settings.iChatFontSize);
		ini_table_create_entry_as_int(config, "gui", "ChatMaxMessages", m_Settings.iChatMaxMessages);

		ini_table_create_entry_as_int(config, "gui", "cutout", m_Settings.iCutout);
		ini_table_create_entry_as_int(config, "gui", "androidKeyboard", m_Settings.iAndroidKeyboard);
		ini_table_create_entry_as_int(config, "gui", "outfit", m_Settings.iOutfitGuns);
		ini_table_create_entry_as_int(config, "gui", "radarrect", m_Settings.iRadarRect);

		// ini_table_create_entry_as_int(config, "gui", "hparmourtext", m_Settings.iHPArmourText);
		// ini_table_create_entry_as_int(config, "gui", "pcmoney", m_Settings.iPCMoney);
		// ini_table_create_entry_as_int(config, "gui", "ctimecyc", m_Settings.iSkyBox);
		// ini_table_create_entry_as_int(config, "gui", "snow", m_Settings.iSnow);
	}

	if (iIgnoreCategory != 2)
	{
		for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), "hud_color_%d", i);
			if (CAdjustableHudColors::IsUsingHudColor((E_HUD_ELEMENT)i))
			{
				ini_table_create_entry(config, "hud", buff, CAdjustableHudColors::GetHudColorString((E_HUD_ELEMENT)i).c_str());
			}
		}
	}

	if (iIgnoreCategory != 2)
	{
		for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE - 2; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), "hud_position_x_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), "hud_position_y_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).Y);
		}

		for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE - 2; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), "hud_scale_x_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), "hud_scale_y_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).Y);
		}
	}

	if (iIgnoreCategory != 2)
	{
		for (int i = 10; i < E_HUD_ELEMENT::HUD_SIZE; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), "hud_position_x_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), "hud_position_y_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)i).Y);
		}

		for (int i = 10; i < E_HUD_ELEMENT::HUD_SIZE; i++)
		{
			char buff[30];
			snprintf(buff, sizeof(buff), "hud_scale_x_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).X);

			snprintf(buff, sizeof(buff), "hud_scale_y_%d", i);
			ini_table_create_entry_as_int(config, "hud", buff, CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)i).Y);
		}
	}

	ini_table_write_to_file(config, buff);
	ini_table_destroy(config);
}

const stSettings &CSettings::GetReadOnly()
{
	return m_Settings;
}

stSettings &CSettings::GetWrite()
{
	Save();
	return m_Settings;
}

void CSettings::LoadSettings(const char *szNickName, int iChatLines)
{
	char tempNick[40];
	if (szNickName)
	{
		strcpy(tempNick, szNickName);
	}

	Log("Loading settings..");

	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);

	ini_table_s *config = ini_table_create();
	Log("Opening settings: %s", buff);
	if (!ini_table_read_from_file(config, buff))
	{
		Log("Cannot load settings, exiting...");
		std::terminate();
		return;
	}

	snprintf(m_Settings.szNickName, sizeof(m_Settings.szNickName), "__android_%d%d", rand() % 1000, rand() % 1000);
	memset(m_Settings.szPassword, 0, sizeof(m_Settings.szPassword));
			memset(m_Settings.player_password, 0, sizeof(m_Settings.player_password));
	snprintf(m_Settings.szFont, sizeof(m_Settings.szFont), "visby-round-cf-extra-bold.ttf");

	const char *szName = ini_table_get_entry(config, "client", "name");
	const char *szPassword = ini_table_get_entry(config, "client", "password");\
	const char *pPassword = ini_table_get_entry(config, "client", "player_password");

	m_Settings.szAutoLogin = ini_table_get_entry_as_int(config, "client", "autologin", 0);
	m_Settings.szServer = ini_table_get_entry_as_int(config, "client", "server", 0);
	m_Settings.szDebug = ini_table_get_entry_as_int(config, "client", "debug", 0);
	m_Settings.szHeadMove = ini_table_get_entry_as_int(config, "client", "headmove", 0);
	m_Settings.szDL = ini_table_get_entry_as_int(config, "client", "dl", 0);
	m_Settings.szTimeStamp = ini_table_get_entry_as_int(config, "client", "timestamp", 0);

	const char *szFontName = ini_table_get_entry(config, "gui", "Font");

	if(pPassword)
	{
		strcpy(m_Settings.player_password, pPassword);
	}
	if (szName)
	{
		strcpy(m_Settings.szNickName, szName);
	}
	if (szPassword)
	{
		strcpy(m_Settings.szPassword, szPassword);
	}
	if (szFontName)
	{
		strcpy(m_Settings.szFont, szFontName);
	}

	ClearBackslashN(m_Settings.szNickName, sizeof(m_Settings.szNickName));
	ClearBackslashN(m_Settings.szPassword, sizeof(m_Settings.szPassword));
	ClearBackslashN(m_Settings.szFont, sizeof(m_Settings.szFont));
	ClearBackslashN(m_Settings.player_password, sizeof(m_Settings.player_password));

	if (szNickName)
	{
		strcpy(m_Settings.szNickName, tempNick);
	}

	m_Settings.fFontSize = ini_table_get_entry_as_float(config, "gui", "FontSize", 30.0f);
	m_Settings.iChatFontSize = ini_table_get_entry_as_int(config, "gui", "ChatFontSize", -1);
	m_Settings.iFontOutline = ini_table_get_entry_as_int(config, "gui", "FontOutline", 2);

	m_Settings.iChatMaxMessages = ini_table_get_entry_as_int(config, "gui", "ChatMaxMessages", -1);

	m_Settings.fButtonMicrophoneSize = ini_table_get_entry_as_float(config, "gui", "MicrophoneSize", 130.0f);
	m_Settings.fButtonMicrophoneX = ini_table_get_entry_as_float(config, "gui", "MicrophoneX", 1650.0f);
	m_Settings.fButtonMicrophoneY = ini_table_get_entry_as_float(config, "gui", "MicrophoneY", 620.0f);

	m_Settings.fButtonEnterPassengerSize = ini_table_get_entry_as_float(config, "gui", "fButtonEnterPassengerSize", 130.0f);
	m_Settings.fButtonEnterPassengerX = ini_table_get_entry_as_float(config, "gui", "ButtonEnterPassengerX", 1650.0f);
	m_Settings.fButtonEnterPassengerY = ini_table_get_entry_as_float(config, "gui", "ButtonEnterPassengerY", 620.0f);

	m_Settings.fButtonCameraCycleX = ini_table_get_entry_as_float(config, "gui", "CameraCycleX", 360.0f);
	m_Settings.fButtonCameraCycleY = ini_table_get_entry_as_float(config, "gui", "CameraCycleY", 960.0f);
	m_Settings.fButtonCameraCycleSize = ini_table_get_entry_as_float(config, "gui", "CameraCycleSize", 90.0f);

	m_Settings.fScoreBoardSizeX = ini_table_get_entry_as_float(config, "gui", "ScoreBoardSizeX", 1024.0f);
	m_Settings.fScoreBoardSizeY = ini_table_get_entry_as_float(config, "gui", "ScoreBoardSizeY", 768.0f);

	m_Settings.iFPS = ini_table_get_entry_as_int(config, "gui", "fps", 60);

	m_Settings.iCutout = ini_table_get_entry_as_int(config, "gui", "cutout", 0);
	m_Settings.iAndroidKeyboard = ini_table_get_entry_as_int(config, "gui", "androidKeyboard", 0);

	m_Settings.iOutfitGuns = ini_table_get_entry_as_int(config, "gui", "outfit", 1);
	m_Settings.iRadarRect = ini_table_get_entry_as_int(config, "gui", "radarrect", 0);
	m_Settings.iHPArmourText = ini_table_get_entry_as_int(config, "gui", "hparmourtext", 0);
	m_Settings.iSkyBox = ini_table_get_entry_as_int(config, "gui", "ctimecyc", 1);
	m_Settings.iSnow = ini_table_get_entry_as_int(config, "gui", "snow", 1);

	for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
	{
		char buff[30];
		snprintf(buff, sizeof(buff), "hud_color_%d", i);
		const char *szInput = ini_table_get_entry(config, "hud", buff);
		if (szInput)
		{
			strcpy(buff, szInput);

			ClearBackslashN(buff, sizeof(buff));

			std::string szTemp(buff + 1);

			CAdjustableHudColors::SetHudColorFromString((E_HUD_ELEMENT)i, szTemp);
		}
		else
		{
			CAdjustableHudColors::SetHudColorFromRGBA((E_HUD_ELEMENT)i, -1, -1, -1, -1);
		}
	}

	for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
	{
		char buff[30];
		snprintf(buff, sizeof(buff), "hud_position_x_%d", i);
		int valueX = ini_table_get_entry_as_int(config, "hud", buff, -1);

		snprintf(buff, sizeof(buff), "hud_position_y_%d", i);
		int valueY = ini_table_get_entry_as_int(config, "hud", buff, -1);

		CAdjustableHudPosition::SetElementPosition((E_HUD_ELEMENT)i, valueX, valueY);
		CAdjustableHudPosition::SetElementPosition((HUD_RADAR), 97, 85); //97, 85

	}

	for (int i = 0; i < E_HUD_ELEMENT::HUD_SIZE; i++)
	{
		char buff[30];
		snprintf(buff, sizeof(buff), "hud_scale_x_%d", i);
		int valueX = ini_table_get_entry_as_int(config, "hud", buff, -1);

		snprintf(buff, sizeof(buff), "hud_scale_y_%d", i);
		int valueY = ini_table_get_entry_as_int(config, "hud", buff, -1);

		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)i, valueX, valueY);
		CAdjustableHudScale::SetElementScale((HUD_RADAR), 73, -1);
	}

	ini_table_destroy(config);
}

extern CSettings* pSettings;

extern "C"
{

JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AuthorizationManager_ToggleAutoLogin(JNIEnv *env, jobject thiz,
																jboolean toggle) {
	pSettings->GetWrite().szAutoLogin = toggle;
	pSettings->Save();
}

JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHpArmourText(JNIEnv *pEnv, jobject thiz,
																   jboolean b) {
	if (pSettings) {
		pSettings->GetWrite().iHPArmourText = b;
	}
	pHud->ToggleHpText(b);
	pSettings->Save();
	//CInfoBarText::SetEnabled(b);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_core_DialogClientSettingsCommonFragment_ChatLineChanged(JNIEnv *env,
																			   jobject thiz,
																			   jint newcount) {
	if (pSettings)
	{
		pSettings->GetWrite().iChatMaxMessages = newcount;
		pSettings->Save();
	}
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_core_DialogClientSettingsCommonFragment_ChatFontSizeChanged(JNIEnv *env,
																				   jobject thiz,
																				   jint size) {
	if (pSettings)
	{
		pSettings->GetWrite().iChatFontSize = size;
		pSettings->Save();
        pHud->ChangeChatTextSize(size);
	}
}

}
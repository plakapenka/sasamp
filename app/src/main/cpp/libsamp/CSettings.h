#pragma once

#define MAX_SETTINGS_STRING	0x7F

static struct stSettings
{
	// client
	char szNickName[MAX_PLAYER_NAME+1];
	char player_password[66];
	char szPassword[66];
	int szAutoLogin;
	int iIsEnableDamageInformer;
	int iIsEnable3dTextInVehicle;
	int szServer;
	int szDebug;
	int szHeadMove;
	int szDL;
	int szTimeStamp;

	// gui
	char szFont[40];
	float fFontSize;
	int iChatFontSize;
	int iFontOutline;

	int iChatMaxMessages;

	int iFPS;

	int iAndroidKeyboard;
	int iOutfitGuns;
	int isTestMode;
	int iHPArmourText;
};

static class CSettings
{
public:
    static stSettings& Get() { return m_Settings; }
	static void toDefaults(int iCategory);

	static static void save(int iIgnoreCategory = 0);

	static void LoadSettings(const char* szNickName, int iChatLines = -1);

	static stSettings m_Settings;
};
#pragma once

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// client
	char szNickName[MAX_PLAYER_NAME+1];
	char player_password[66];
	char szPassword[66];
	int szAutoLogin;
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
	int iCutout;
	int iOutfitGuns;
	int iRadarRect;
	int iHPArmourText;
	int iSkyBox;
	int iSnow;
};

class CSettings
{
public:
	CSettings();
	~CSettings();
    stSettings& Get() { return m_Settings; }
	void ToDefaults(int iCategory);

	void Save(int iIgnoreCategory = 0);

	const stSettings& GetReadOnly();
	stSettings& GetWrite();
	void LoadSettings(const char* szNickName, int iChatLines = -1);
private:
	stSettings m_Settings;
};
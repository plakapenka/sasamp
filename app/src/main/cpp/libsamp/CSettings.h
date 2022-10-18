#pragma once

#define MAX_SETTINGS_STRING	0x7F

struct stSettings
{
	// client
	char szNickName[MAX_PLAYER_NAME+1];
	char szPassword[MAX_SETTINGS_STRING+1];
	int szAutoLogin;
	int szServer;
	int szDebug;
	int szHeadMove;
	int szDL;
	int szTimeStamp;

	// gui
	char szFont[40];
	float fFontSize;
	int iFontOutline;

	float fChatPosX;
	float fChatPosY;
	float fChatSizeX;
	float fChatSizeY;
	int iChatMaxMessages;

	float fHealthBarWidth;
	float fHealthBarHeight;

	int iFPS;

	float fButtonMicrophoneX;
	float fButtonMicrophoneY;
	float fButtonMicrophoneSize;

	float fButtonEnterPassengerX;
	float fButtonEnterPassengerY;
	float fButtonEnterPassengerSize;

	float fButtonCameraCycleX;
	float fButtonCameraCycleY;
	float fButtonCameraCycleSize;

	float fScoreBoardSizeX;
	float fScoreBoardSizeY;

	int iVoiceList;

	int iAndroidKeyboard;
	int iCutout;
	int iOutfitGuns;
	int iRadarRect;
	int iHPArmourText;
	int iPCMoney;
	int iSkyBox;
	int iSnow;
	
	int iDialog;
	int iHud;
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
	void LoadSettings(const char* szNickName, int iChatLines = 8);
private:
	stSettings m_Settings;
};
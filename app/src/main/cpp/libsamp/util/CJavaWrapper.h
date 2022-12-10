#pragma once

#include <jni.h>

#include <string>

extern
#define EXCEPTION_CHECK(env) \
	if ((env)->ExceptionCheck()) \ 
	{ \
		(env)->ExceptionDescribe(); \
		(env)->ExceptionClear(); \
		return; \
	}

class CJavaWrapper
{
	jobject activity;

	jmethodID s_GetClipboardText;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;

	jmethodID s_showOilFactoryGame;
	jmethodID s_showArmyGame;
	jmethodID s_hideArmyGame;

	jmethodID s_showFuelStation;

	jmethodID s_ToggleShopStoreManager;

	jmethodID s_showGunShopManager;
	jmethodID s_hideGunShopManager;

	jmethodID s_showGreenZone;
    jmethodID s_hideGreenZone;
	jmethodID s_showGPS;
    jmethodID s_hideGPS;
	jmethodID s_updateLevelInfo;

	jmethodID s_showSamwill;
	jmethodID s_hideSamwill;

	jmethodID s_showSpeed;
    jmethodID s_hideSpeed;
	jmethodID s_updateSpeedInfo;

	jmethodID s_showNotification;
	jmethodID s_hideNotification;
	jmethodID s_showMenu;

	jmethodID s_showAuthorization;
	jmethodID s_hideAuthorization;

	jmethodID s_showRegistration;
	jmethodID s_hideRegistration;

	jmethodID j_showDeathInfo;

	jmethodID j_toggleAutoShop;
	jmethodID j_updateAutoShop;

	jmethodID s_showChooseSpawn;
	jmethodID s_hideChooseSpawn;

	jmethodID s_setPauseState;

	//jmethodID s_showSplash;
	jmethodID s_updateSplash;
	jmethodID s_ExitGame;

	jmethodID s_hideServerLogo;
	jmethodID s_showServerLogo;

	jmethodID s_showBusInfo;
	jmethodID s_hideBusInfo;

	jmethodID s_showYernMoney;
	jmethodID s_hideYernMoney;
	jmethodID s_updateYearnMoney;
	jmethodID s_showUpdateTargetNotify;
	jmethodID s_hideTargetNotify;


public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();

	void ShowClientSettings();

	void SetUseFullScreen(int b);

	void UpdateLevelInfo(int level, int currentexp, int maxexp);

    void ToggleAllHud(bool toggle);

	void ShowGreenZone();
    void HideGreenZone();
	void ShowGPS();
    void HideGPS();
	void HideServerLogo();
	void ShowServerLogo();

	void SetTabStat(int id, char* name, int score, int ping);
	void ShowTabWindow();

	void ShowOilFactoryGame();
	void ShowArmyGame(int quantity);
	void HideArmyGame();

	void ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount);

	void ToggleShopStoreManager(bool toggle, int type = 0, int price = 0);

	void ShowGunShopManager();
	void HideGunShopManager();

	void ShowSamwill();
	void HideSamwill();

	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowSpeed();
    void HideSpeed();

	void ShowBusInfo(int time);
	void HideBusInfo();

	void ShowYernMoney();
    void HideYernMoney();
    void UpdateYearnMoney(int money);
    void ShowUpdateTargetNotify(int type, char *text);
    void HideTargetNotify();

	void MakeDialog(int dialogId, int dialogTypeId, char caption[], char info[], char button1[], char button2[]); // Диалоги
	void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn, int actionId);
	void HideNotification();
	void ShowMenu();

	void ShowAuthorization(char *nick, int id, bool ip_match, bool toggleAutoLogin, bool email_acvive);
	void HideAuthorization();

	void ShowDeathInfo(std::string nick, int id);
	void HideDeathInfo();

	void ShowRegistration(char *nick, int id);
	void HideRegistration();
	void TempToggleCasinoDice(bool toggle);
	void ShowCasinoDice(bool show, int tableID, int tableBet, int tableBank, int money, char player1name[], int player1stat, char player2name[], int player2stat, char player3name[], int player3stat, char player4name[], int player4stat, char player5name[], int player5stat);

	void ShowChooseSpawn(int organization, int station, int exit, int garage, int house);
	void HideChooseSpawn();

	void SetPauseState(bool a1);

	//void ShowSplash();
	void UpdateSplash(int progress);	

	uint32_t ChangeRegisterSkin(int skin);

	int RegisterSexMale;
	int RegisterSkinValue;
	int RegisterSkinId;

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

	void UpdateAutoShop(const char name[], int price, int count, float maxspeed, float acceleration);

	void ToggleAutoShop(bool toggle);

	void ClearScreen();

	jobject jCasinoDice;
    jobject jDialog;
	jobject jCasino_LuckyWheel;

    void ShowCasinoLuckyWheel(int count, int time);

    void ShowMiningGame1(bool toggle);

	void ShowMiningGame2(bool toggle);

    void ShowMiningGame3(bool toggle);

	void ExitGame();
};

extern CJavaWrapper* g_pJavaWrapper;
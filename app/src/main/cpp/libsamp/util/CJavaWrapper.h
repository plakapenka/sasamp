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

	jmethodID s_GetClipboardText;
	jmethodID j_Vibrate;

	jmethodID s_ShowClientSettings;

	jmethodID s_showOilFactoryGame;
	jmethodID s_showArmyGame;
	jmethodID s_hideArmyGame;

	jmethodID s_showFuelStation;

	jmethodID s_ToggleShopStoreManager;

	jmethodID s_showGunShopManager;
	jmethodID s_hideGunShopManager;

	jmethodID s_showSamwill;

	jmethodID s_showMenu;

	jmethodID s_showAuthorization;
	jmethodID s_hideAuthorization;

	jmethodID s_showRegistration;
	jmethodID s_hideRegistration;

	jmethodID j_toggleAutoShop;
	jmethodID j_updateAutoShop;

	jmethodID s_showChooseSpawn;
	jmethodID s_hideChooseSpawn;

	jmethodID s_setPauseState;

	//jmethodID s_showSplash;
	jmethodID s_updateSplash;
	jmethodID s_ExitGame;


public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();

	void ShowClientSettings();

	void ShowOilFactoryGame();
	void ShowArmyGame(int quantity);
	void HideArmyGame();

	void ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount);

	void ToggleShopStoreManager(bool toggle, int type = 0, int price = 0);

	void ShowGunShopManager();
	void HideGunShopManager();

	void ShowSamwill();

	void ShowMenu();

	void ShowAuthorization(char *nick, int id, bool ip_match, bool toggleAutoLogin, bool email_acvive);
	void HideAuthorization();

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

	void UpdateAutoShop(const char name[], int price, int count, float maxspeed, float acceleration, int gear);

	void ToggleAutoShop(bool toggle);

	void ClearScreen();

	jobject jCasinoDice;
	jobject jCasino_LuckyWheel;

    void ShowCasinoLuckyWheel(int count, int time);

    void ShowMiningGame1(bool toggle);

	void ShowMiningGame2(bool toggle);

    void ShowMiningGame3(bool toggle);

	void ExitGame();

	void Vibrate(int milliseconds);

    void SendBuffer(const char string[]);

	jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;
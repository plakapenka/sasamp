#pragma once

#include <jni.h>

#include <string>

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
	jmethodID s_CallLauncherActivity;

	jmethodID s_ShowInputLayout;
	jmethodID s_HideInputLayout;

	jmethodID s_ShowClientSettings;
	jmethodID s_SetUseFullScreen;
	jmethodID s_MakeDialog;

	jmethodID s_showOilFactoryGame;
	jmethodID s_showArmyGame;
	jmethodID s_hideArmyGame;

	jmethodID s_showFuelStation;

	jmethodID s_showShopStoreManager;
	jmethodID s_hideShopStoreManager;

	jmethodID s_showGunShopManager;
	jmethodID s_hideGunShopManager;

	jmethodID s_showTabWindow;
	jmethodID s_setTabStat;

	jmethodID s_showHud;
    jmethodID s_hideHud;
	jmethodID s_showHudDialog;
    jmethodID s_hideHudDialog;
	jmethodID s_showGreenZone;
    jmethodID s_hideGreenZone;
	jmethodID s_showGPS;
    jmethodID s_hideGPS;
	jmethodID s_showServer;
    jmethodID s_hideServer;
	jmethodID s_updateHudInfo;
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

	jmethodID s_showDeathInfo;
	jmethodID s_hideDeathInfo;

	jmethodID s_showAuctionManager;

	jmethodID s_showChooseSpawn;
	jmethodID s_hideChooseSpawn;

	jmethodID s_setPauseState;

	jmethodID s_showSplash;
	jmethodID s_updateSplash;

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
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowClientSettings();

	void SetUseFullScreen(int b);

	void UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoinclip, int money, int wanted);
	void UpdateLevelInfo(int level, int currentexp, int maxexp);
	void ShowHud();
    void HideHud();
	void ShowHudDialog();
    void HideHudDialog();
	void ShowGreenZone();
    void HideGreenZone();
	void ShowGPS();
    void HideGPS();
	void ShowServer(int serverid);
    void HideServer();

	void SetTabStat(int id, char* name, int score, int ping);
	void ShowTabWindow();

	void ShowOilFactoryGame();
	void ShowArmyGame(int quantity);
	void HideArmyGame();

	void ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5);

	void ShowAuctionManager(int itemId, int type, int price);

	void ShowShopStoreManager(int type, int price);
	void HideShopStoreManager();

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

	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	void ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn, int actionId);
	void HideNotification();
	void ShowMenu();

	void ShowAuthorization(char *nick, int id);
	void HideAuthorization();

	void ShowDeathInfo(char *nick, int id);
	void HideDeathInfo();

	void ShowRegistration(char *nick, int id);
	void HideRegistration();

	void ShowChooseSpawn(int organization, int station, int exit, int garage, int house);
	void HideChooseSpawn();

	void SetPauseState(bool a1);

	void ShowSplash();
	void UpdateSplash(int progress);	

	uint32_t ChangeRegisterSkin(int skin);

	bool isHudToggle;

	int RegisterSexMale;
	int RegisterSkinValue;
	int RegisterSkinId;

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;
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
	jmethodID s_hideDialogWithoutReset;


	jmethodID s_showLogin;
    jmethodID s_hideLogin;
	jmethodID s_showLoginTwo;
    jmethodID s_hideLoginTwo;
	jmethodID s_showRegister;
    jmethodID s_hideRegister;
	jmethodID s_showRegisterTwo;
    jmethodID s_hideRegisterTwo;
	jmethodID s_showRegisterSex;
    jmethodID s_hideRegisterSex;
	jmethodID s_showRegisterSkin;
    jmethodID s_hideRegisterSkin;
	jmethodID s_updateRegisterSex;
	jmethodID s_showSpawn;
    jmethodID s_hideSpawn;
	jmethodID s_hideLoadingScreen;
	jmethodID s_updateLoadingScreen;
	jmethodID s_showHud;
    jmethodID s_hideHud;
	jmethodID s_showVoice;
    jmethodID s_hideVoice;
	jmethodID s_showServer;
    jmethodID s_hideServer;
	jmethodID s_showGPS;
    jmethodID s_hideGPS;
	jmethodID s_showGreenZone;
    jmethodID s_hideGreenZone;
	jmethodID s_showSamwill;
    jmethodID s_hideSamwill;
	jmethodID s_showSamwillMoney;
    jmethodID s_hideSamwillMoney;
	jmethodID s_showMenu;
    jmethodID s_hideMenu;
	jmethodID s_updateHudInfo;
	jmethodID s_updateSamwill;
	jmethodID s_updateRegisterSkinButtons;
	jmethodID s_updateSamwillMoney;
	jmethodID s_showSpeedometr;
    jmethodID s_hideSpeedometr;
	jmethodID s_updateSpeedometr;
	jmethodID s_showKilllist;
	jmethodID s_hideKilllist;
	jmethodID s_showKilllistMulti;
	jmethodID s_hideKilllistMulti;
	jmethodID s_showKilllisDuels;
	jmethodID s_hideKilllistDuels;
	jmethodID s_updateKilllist;
	jmethodID s_updateKilllistOne;
	jmethodID s_updateKilllistTwo;
	jmethodID s_updateKilllistThree;
	jmethodID s_clearKilllistDuels;
	jmethodID s_hideKilllistOne;
	jmethodID s_hideKilllistTwo;
	jmethodID s_hideKilllistThree;
	jmethodID s_updateKilllistTop;
	jmethodID s_hideTargetNotify;
	jmethodID s_updateTargetNotify;
	jmethodID s_showNotification;
	jmethodID s_updateNotification;
	jmethodID s_hideNotification;
	jmethodID s_updateAutosalon;
	jmethodID s_hideAutosalon;
	jmethodID s_showAutosalon;
	jmethodID s_showZavod;
	jmethodID s_hideZavod;
	jmethodID s_showMine;
	jmethodID s_hideMine;
	jmethodID s_showUpdateBus;
	jmethodID s_hideBus;
	jmethodID s_showUpdateCasino;
	jmethodID s_hideCasino;
	jmethodID s_hideCasinoChip;
	jmethodID s_showCasinoChip;
	jmethodID s_showCasinoRoullete;
	jmethodID s_hideCasinoRoullete;

	jmethodID s_setPauseState;
public:
	JNIEnv* GetEnv();

	std::string GetClipboardString();
	void CallLauncherActivity(int type);

	void ShowInputLayout();
	void HideInputLayout();

	void ShowClientSettings();

	void UpdateHudInfo(int health, int armour, int level, int levelcurrent, int levelmax, int weaponid, int ammo, int ammoinclip, int money, int wanted);
	void UpdateSamwill(int start, int progress, int check1, int check2, int check3, int check4, int check5);
	void UpdateSamwillMoney(int money);
	void UpdateRegisterSkinButtons(int btnidclicked);
	void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock);
	void ShowLogin();
    void HideLogin();
	void ShowLoginTwo();
    void HideLoginTwo();
	void ShowRegister();
    void HideRegister();
	void ShowRegisterTwo();
    void HideRegisterTwo();
	void ShowRegisterSex();
    void HideRegisterSex();
	void ShowRegisterSkin();
    void HideRegisterSkin();
	void UpdateRegisterSex(int male);
	void ShowSpawn();
    void HideSpawn();
	void HideLoadingScreen();
	void UpdateLoadingScreen(int procent);
	void UpdateTargetNotify(int type, char *text);
	void HideTargetNotify();
	void HideNotification();
	void ShowNotification(int type, char *text);
	void UpdateNotification(int progress);
	void HideBus();
	void ShowUpdateBus(int timer);
	void HideCasinoRoullete();
	void ShowCasinoRoullete(int quantity);
	void HideCasinoChip();
	void ShowCasinoChip(int type, int balance);
	void HideCasino();
	void ShowUpdateCasino(int tableid, int bet, int bank, int balance, char* playerid1, int playerstatus1, char* playerid2, int playerstatus2, char* playerid3, int playerstatus3, char* playerid4, int playerstatus4, char* playerid5, int playerstatus5);
	void UpdateAutosalon(char *vehname, int price, int quantity, int maxspeed, int overclocking);
	void HideAutosalon();
	void ShowAutosalon();
	void ShowZavod(int type);
	void HideZavod();
	void ShowMine(int type);
	void HideMine();
	void ShowHud();
    void HideHud();
	void ShowVoice();
    void HideVoice();
	void ShowServer(int server);
    void HideServer();
	void ShowGPS();
    void HideGPS();
	void ShowGreenZone();
    void HideGreenZone();
	void ShowSamwill();
    void HideSamwill();
	void ShowSamwillMoney();
    void HideSamwillMoney();
	void ShowMenu();
    void HideMenu();
	void ShowSpeedometr();
    void HideSpeedometr();
	uint32_t ChangeRegisterSkin(int skin);
	void ClearKilllistDuels();
	void HideKilllistOne();
	void HideKilllistTwo();
	void HideKilllistThree();

	void UpdateKilllistOne(int team, int weapon, char *killer, char *killed);
	void UpdateKilllistTwo(int team, int weapon, char *killer, char *killed);
	void UpdateKilllistThree(int team, int weapon, char *killer, char *killed);

	void UpdateKilllistTop(char *one, char *two, char *three);

	void ShowKilllist();
	void HideKilllist();
	void ShowKilllistMulti();
	void HideKilllistMulti();
	void ShowKilllisDuels();
	void HideKilllistDuels();
	void UpdateKilllist(int kills, int deaths, int duelskills, int duelkillsmax);

	void SetUseFullScreen(int b);
	void MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText); // Диалоги
	void HideDialogWithoutReset();

	void SetPauseState(bool a1);

	int RegisterSexMale;
	int RegisterSkinValue;
	int RegisterSkinId;

	int ChooseSpawn;

	uint16_t CasinoWheelID;

	bool isHudToggle;

	bool isDialogActive;

	bool SpeedLeftTurn;
	bool SpeedRightTurn;
	bool SpeedTurn;

	bool Samwillclicked;
	int Samwillcheck1;
	int Samwillcheck2;
	int Samwillcheck3;
	int Samwillcheck4;
	int Samwillcheck5;

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();
};

extern CJavaWrapper* g_pJavaWrapper;

// edited source code by x 1 y 2 z
// if you delete this code i fuck ur mom
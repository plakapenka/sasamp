#include "CJavaWrapper.h"
#include "../main.h"

extern "C" JavaVM* javaVM;

#include "..//keyboard.h"
#include "..//CSettings.h"
#include "..//net/netgame.h"
#include "../game/game.h"
#include "../str_obfuscator_no_template.hpp"
#include "java_systems/CTab.h"
#include "java_systems/CHUD.h"

extern CNetGame* pNetGame;
extern CGame* pGame;

JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
	{
		Log("GetEnv: not attached");
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			Log("Failed to attach");
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		Log("GetEnv: version not supported");
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		Log("GetEnv: JNI_ERR");
		return nullptr;
	}

	return env;
}

void CJavaWrapper::ShowClientSettings()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowClientSettings);

	EXCEPTION_CHECK(env);
}

#include "..//CDebugInfo.h"
#include "chatwindow.h"
#include "java_systems/CMedic.h"
#include "java_systems/CSpeedometr.h"
#include "java_systems/CChooseSpawn.h"

void CJavaWrapper::ShowFuelStation(int type, int price1, int price2, int price3, int price4, int price5, int maxCount)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showFuelStation, type, price1, price2, price3, price4, price5, maxCount);
}

void CJavaWrapper::ShowGunShopManager()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showGunShopManager);
}

void CJavaWrapper::HideGunShopManager()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideGunShopManager);
}

void CJavaWrapper::ToggleShopStoreManager(bool toggle, int type, int price)
{
	pGame->isShopStoreActive = toggle;
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_ToggleShopStoreManager, toggle, type, price);
}

void CJavaWrapper::ShowArmyGame(int quantity)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showArmyGame, quantity);
}

void CJavaWrapper::HideArmyGame()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideArmyGame);
}

void CJavaWrapper::ShowOilFactoryGame()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showOilFactoryGame);
}

void CJavaWrapper::ShowSamwill()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showSamwill);
}

void CJavaWrapper::Vibrate(int milliseconds)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->j_Vibrate, milliseconds);
}

void CJavaWrapper::SetPauseState(bool a1)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_setPauseState, a1);
}

//void CJavaWrapper::ShowSplash() {
//
//	JNIEnv* env = GetEnv();
//
//	if (!env)
//	{
//		Log("No env");
//		return;
//	}
//
//	env->CallVoidMethod(this->activity, this->s_showSplash);
//}

void CJavaWrapper::ExitGame() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_ExitGame);
}

void CJavaWrapper::ShowMenu() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showMenu);
}

void CJavaWrapper::ToggleAutoShop(bool toggle)
{
	JNIEnv* env = GetEnv();

    if (!env)
	{
		Log("No env");
		return;
	}
	pGame->isAutoShopActive = toggle;

	env->CallVoidMethod(this->activity, this->j_toggleAutoShop, toggle);
}
void CJavaWrapper::UpdateAutoShop(const char name[], int price, int count, float maxspeed, float acceleration, int gear)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jstring j_name = env->NewStringUTF( name );

	env->CallVoidMethod(this->activity, this->j_updateAutoShop, j_name, price, count, maxspeed, acceleration, gear);

	env->DeleteLocalRef(j_name);
}

void CJavaWrapper::ClearScreen()
{
	Log("ClearScreen");
	ShowMiningGame1(false);
	ShowMiningGame2(false);
	ShowMiningGame3(false);
	ToggleShopStoreManager(false);
	CHUD::hideTargetNotify();
	CChooseSpawn::hide();
	CSpeedometr::hide();
	HideArmyGame();
	this->ToggleAutoShop(false);
	CHUD::hideBusInfo();
	CHUD::toggleGps(false);
	CHUD::toggleGreenZone(false);
	CMedic::hide();

	ShowCasinoDice(false, 0, 0, 0, 0, "--", 0, "--", 0, "--", 0, "--", 0, "--", 0);
}

const uint32_t cRegisterSkin[2][10] = {
	{ 9, 195, 231, 232, 1, 1, 1, 1, 1, 1 }, // female
	{ 16, 79, 134, 135, 200, 234, 235, 236, 239 } // male
};

uint32_t CJavaWrapper::ChangeRegisterSkin(int skin)
{
	uint32_t uiSkin = 16;
	bool bIsMan = g_pJavaWrapper->RegisterSexMale == 1 ? true : false;
	uint32_t uiMaxSkins = bIsMan ? 9 : 4;

	if (!(0 < skin <= uiMaxSkins)) {
		g_pJavaWrapper->RegisterSkinId = uiSkin;
		return uiSkin;
	}

	uiSkin = cRegisterSkin[(int)bIsMan][skin - 1];
	g_pJavaWrapper->RegisterSkinId = uiSkin;

	return uiSkin;
}

CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);

	if (!nvEventClass)
	{
		Log("nvEventClass null");
		return;
	}

	s_ShowClientSettings = env->GetMethodID(nvEventClass, "showClientSettings", "()V");

	s_showOilFactoryGame = env->GetMethodID(nvEventClass, "showOilFactoryGame", "()V");
	s_showArmyGame = env->GetMethodID(nvEventClass, "showArmyGame", "(I)V");
	s_hideArmyGame = env->GetMethodID(nvEventClass, "hideArmyGame", "()V");

	s_showFuelStation = env->GetMethodID(nvEventClass, "showFuelStation", "(IIIIIII)V");

	s_ToggleShopStoreManager = env->GetMethodID(nvEventClass, "toggleShopStoreManager", "(ZII)V");

	s_showGunShopManager = env->GetMethodID(nvEventClass, "showGunShopManager", "()V");
	s_hideGunShopManager = env->GetMethodID(nvEventClass, "hideGunShopManager", "()V");

	j_Vibrate = env->GetMethodID(nvEventClass, "goVibrate", "(I)V");

    s_showSamwill = env->GetMethodID(nvEventClass, "showSamwill", "()V");

	s_showMenu = env->GetMethodID(nvEventClass, "showMenu", "()V");

	j_toggleAutoShop = env->GetMethodID(nvEventClass, "toggleAutoShop", "(Z)V");

	j_updateAutoShop = env->GetMethodID(nvEventClass, "updateAutoShop", "(Ljava/lang/String;IIFFI)V");

	s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

	s_ExitGame = env->GetMethodID(nvEventClass, "ExitGame", "()V");
	//s_showSplash = env->GetMethodID(nvEventClass, "showSplash", "()V");

	env->DeleteLocalRef(nvEventClass);
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

void CJavaWrapper::TempToggleCasinoDice(bool toggle) {
	JNIEnv* env = GetEnv();
	if (!env)
	{
		Log("No env");
		return;
	}

	jclass _class = env->GetObjectClass(jCasinoDice);
	jmethodID TempToggle = env->GetMethodID(_class, "TempToggle", "(Z)V");

	env->CallVoidMethod(jCasinoDice, TempToggle, toggle);

}

void CJavaWrapper::ShowCasinoDice(bool show, int tableID, int tableBet, int tableBank, int money,
								  char player1name[], int player1stat,
								  char player2name[], int player2stat,
								  char player3name[], int player3stat,
								  char player4name[], int player4stat,
								  char player5name[], int player5stat) {


	JNIEnv* env = GetEnv();

    jclass clazz = env->GetObjectClass(jCasinoDice);


	jmethodID Toggle = env->GetMethodID(clazz, "Toggle", "(ZIIIILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;I)V");

	jstring jPlayer1Name = env->NewStringUTF( player1name );
	jstring jPlayer2Name = env->NewStringUTF( player2name );
	jstring jPlayer3Name = env->NewStringUTF( player3name );
	jstring jPlayer4Name = env->NewStringUTF( player4name );
	jstring jPlayer5Name = env->NewStringUTF( player5name );

	env->CallVoidMethod(jCasinoDice, Toggle, show, tableID, tableBet, tableBank, money, jPlayer1Name, player1stat, jPlayer2Name, player2stat, jPlayer3Name, player3stat, jPlayer4Name, player4stat, jPlayer5Name, player5stat);

	env->DeleteLocalRef(jPlayer1Name);
	env->DeleteLocalRef(jPlayer2Name);
	env->DeleteLocalRef(jPlayer3Name);
	env->DeleteLocalRef(jPlayer4Name);
	env->DeleteLocalRef(jPlayer5Name);
}

void CJavaWrapper::ShowCasinoLuckyWheel(int count, int time) {

	pGame->isCasinoWheelActive = true;
	JNIEnv* env = GetEnv();

	jclass clazz = env->GetObjectClass(jCasino_LuckyWheel);
	jmethodID Show = env->GetMethodID(clazz, "show", "(II)V");

	env->CallVoidMethod(jCasino_LuckyWheel, Show, count, time);
}

void CJavaWrapper::SendBuffer(const char string[]) {
	JNIEnv* env = GetEnv();

	jstring jstring = env->NewStringUTF( string );
	//jclass clazz = env->GetObjectClass(jCasino_LuckyWheel);
	jclass nvEventClass = env->GetObjectClass(activity);
	jmethodID CopyTextToBuffer = env->GetMethodID(nvEventClass, "CopyTextToBuffer", "(Ljava/lang/String;)V");

	env->CallVoidMethod(activity, CopyTextToBuffer, jstring);
	env->DeleteLocalRef(jstring);
}

CJavaWrapper* g_pJavaWrapper = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Casino_1LuckyWheel_ClickButt(JNIEnv *env, jobject thiz, jint button_id) {
	pGame->isCasinoWheelActive = false;
	if(button_id == 228)
	{// Закрыл
		return;
	}
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_CASINO_LUCKY_WHEEL_MENU;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_core_Samp_playUrlSound(JNIEnv *env, jclass clazz, jstring url) {
	const char *_url = env->GetStringUTFChars(url, nullptr);

	pNetGame->GetStreamPool()->PlayIndividualStream(_url, BASS_STREAM_AUTOFREE);

	env->ReleaseStringUTFChars(url, _url);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_OilFactoryManager_onOilFactoryGameClose(JNIEnv *env, jobject thiz,
																   jboolean success) {
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_SHOW_OILGAME;

	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write((uint8_t)success);

	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_AutoShop_sendAutoShopButton(JNIEnv *env, jobject thiz, jint button_id) {
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_CLICK_AUTOSHOP;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_SamwillManager_onSamwillHideGame(JNIEnv *env, jobject thiz,
															jint samwillpacket) {
	pNetGame->SendCustomPacket(251, 20, samwillpacket);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_ArmyGameManager_onArmyGameClose(JNIEnv *env, jobject thiz) {
	pNetGame->SendCustomPacket(251, 45, 1);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_FuelStationManager_onFuelStationClick(JNIEnv *env, jobject thiz,
																 jint fueltype, jint fuelliters) {
	pNetGame->SendCustomPacketFuelData(251, 39, fueltype, fuelliters);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_ShopStoreManager_onShopStoreClick(JNIEnv *env, jobject thiz,
															 jint buttonid) {
	pNetGame->SendCustomPacket(251, 42, buttonid);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_GunShopManager_onGunShopClick(JNIEnv *env, jobject thiz, jint weaponid) {
	pNetGame->SendCustomPacket(251, 44, weaponid);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_Menu_sendRPC(JNIEnv *env, jobject thiz, jint type, jbyteArray str,
										jint action) {
	switch(type) {
		case 1:
			switch(action) {
				case 1:
					pNetGame->SendChatCommand("/gps");
					break;
				case 2:
					pNetGame->SendChatCommand("/mm");
					break;
				case 3:
					pNetGame->SendChatCommand("/inv");
					break;
				case 4: {
					//pNetGame->SendChatCommand("/anim");
					pNetGame->SendChatCommand("/anim");
					break;
				}
				case 5:
					pNetGame->SendChatCommand("/donat");
					break;
				case 6:
					pNetGame->SendChatCommand("/car");
					break;
				case 7:
				{
					pNetGame->SendChatCommand("/report");
					break;
				}
				case 8:
				{
					pNetGame->SendChatCommand("/promo");
					break;
				}
				case 9:
				{
					CTab::toggle();
					break;
				}
			}
		case 2:
			switch(action) {
				case 0:
					//if (pChatWindow)
					//	CChatWindow::AddDebugMessage("{bbbbbb}Клиент {ff0000}LIVE RUSSIA{bbbbbb} запущен{ffffff}");
					//pNetGame = new CNetGame(cryptor::create("46.174.49.47", 14).decrypt(), atoi(cryptor::create("7788", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
					//pSettings->GetWrite().szServer = 0;
					break;
			}
	}
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_CasinoDice_SendCasinoButt(JNIEnv *env, jobject thiz, jint button_id) {
	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_SHOW_DICE_TABLE;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_core_Samp_togglePlayer(JNIEnv *env, jobject thiz, jint toggle) {
	if(toggle)
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(false, true);
	else
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(true, true);
}
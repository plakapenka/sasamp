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
#include "java_systems/CSpeedometr.h"

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

void CJavaWrapper::ClearScreen()
{
	Log("ClearScreen");

	CHUD::hideTargetNotify();
	CSpeedometr::hide();
	CHUD::hideBusInfo();
	CHUD::toggleGps(false);
	CHUD::toggleGreenZone(false);

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

	j_Vibrate = env->GetMethodID(nvEventClass, "goVibrate", "(I)V");

	s_showMenu = env->GetMethodID(nvEventClass, "showMenu", "()V");

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
Java_com_sasamp_cr_core_Samp_playUrlSound(JNIEnv *env, jclass clazz, jstring url) {
	const char *_url = env->GetStringUTFChars(url, nullptr);

	pNetGame->GetStreamPool()->PlayIndividualStream(_url, BASS_STREAM_AUTOFREE);

	env->ReleaseStringUTFChars(url, _url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_gui_Menu_sendRPC(JNIEnv *env, jobject thiz, jint type, jbyteArray str,
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
Java_com_sasamp_cr_core_Samp_togglePlayer(JNIEnv *env, jobject thiz, jint toggle) {
	if(toggle)
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(false, true);
	else
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(true, true);
}
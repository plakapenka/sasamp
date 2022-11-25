#include "CJavaWrapper.h"
#include "../main.h"

extern "C" JavaVM* javaVM;

#include "..//keyboard.h"
#include "..//CSettings.h"
#include "..//net/netgame.h"
#include "../game/game.h"
#include "../str_obfuscator_no_template.hpp"
#include "java_systems/scoreboard.h"
#include "java_systems/hud.h"

extern CKeyBoard* pKeyBoard;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CGame* pGame;
extern CHUD *pHud;



JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_4);

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

std::string CJavaWrapper::GetClipboardString()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return std::string("");
	}

	jbyteArray retn = (jbyteArray)env->CallObjectMethod(activity, s_GetClipboardText);

	if ((env)->ExceptionCheck())
	{
		(env)->ExceptionDescribe();
		(env)->ExceptionClear();
		return std::string("");
	}

	if (!retn)
	{
		return std::string("");
	}

	jboolean isCopy = true;

	jbyte* pText = env->GetByteArrayElements(retn, &isCopy);
	jsize length = env->GetArrayLength(retn);

	std::string str((char*)pText, length);

	env->ReleaseByteArrayElements(retn, pText, JNI_ABORT);
	
	return str;
}

void CJavaWrapper::ShowInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_ShowInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_HideInputLayout);

	EXCEPTION_CHECK(env);
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

void CJavaWrapper::MakeDialog(int dialogId, int dialogTypeId, char caption[], char info[], char button1[], char button2[])
{
	pGame->isDialogActive = true;

    JNIEnv* env = GetEnv();
    if (!env)
    {
	Log("No env");
	return;
    }

	jstring j_caption = env->NewStringUTF( caption);
	jstring j_info = env->NewStringUTF( info );
	jstring j_button1 = env->NewStringUTF( button1 );
	jstring j_button2 = env->NewStringUTF( button2 );


	jclass clazz = env->GetObjectClass(jDialog);

	jmethodID Show = env->GetMethodID(clazz, "show", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");

    env->CallVoidMethod(jDialog, Show, dialogId, dialogTypeId, j_caption, j_info, j_button1, j_button2);

	// не уверен что нужно, но внятного ответа в интернетах нет
	env->DeleteLocalRef(j_caption);
	env->DeleteLocalRef(j_info);
	env->DeleteLocalRef(j_button1);
	env->DeleteLocalRef(j_button2);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetUseFullScreen(int b)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_SetUseFullScreen, b);

	EXCEPTION_CHECK(env);
}
extern int g_iStatusDriftChanged;
#include "..//CDebugInfo.h"
#include "chatwindow.h"

extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInputEnd(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		if (pKeyBoard)
		{
			pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i3, jint i, jint i2, jbyteArray str)
	{
		jbyte* pMsg = pEnv->GetByteArrayElements(str, nullptr);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);
		//const char *inputText = pEnv->GetStringUTFChars(str, nullptr);

		if(pNetGame) {
			pNetGame->SendDialogResponse(i, i3, i2, const_cast<char *>(szStr.c_str()));

		}
		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);

		pGame->isDialogActive = false;

		//	Log("sendDialogResponse: inputtext1 - %s, inputText - %s", inputtext1, inputText);

		//pEnv->ReleaseStringUTFChars(str, inputText);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendCommand(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendChatCommand((char*)szStr.c_str());
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendRPC(JNIEnv* pEnv, jobject thiz, jint type, jbyteArray str, jint action)
	{
		switch(type) {
			case 1:
				switch(action) {
					case 398:
						pNetGame->SendChatCommand("/gps");
						break;
					case 1:
						pNetGame->SendChatCommand("/gps");
						break;
					case 2:
						pNetGame->SendChatCommand("/mn");
						break;
					case 4: 
						pNetGame->SendChatCommand("/inv");
						break;
					case 5: {
						//pNetGame->SendChatCommand("/anim");
						pNetGame->SendChatCommand("/anim");	
						break;
					}
					case 6:
						pNetGame->SendChatCommand("/donate");
						break;
					case 7:
						pNetGame->SendChatCommand("/car");
						break;
					case 8:
					{
						ToggleTab();
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
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_togglePlayer(JNIEnv* pEnv, jobject thiz, jint toggle) {
		if(toggle)
			pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(false, true);
		else
			pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(true, true);
	}
	JNIEXPORT jint JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getLastServer(JNIEnv* pEnv, jobject thiz)
	{
		return (jint)pSettings->GetReadOnly().szServer;
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv* pEnv, jobject thiz)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsOpen())
			{
				Log("Closing keyboard");
				pKeyBoard->Close();
			}
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeCutoutSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iCutout = b;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iAndroidKeyboard = b;
		}

		if (pKeyBoard && b)
		{
			pKeyBoard->EnableNewKeyboard();
		}
		else if(pKeyBoard)
		{
			pKeyBoard->EnableOldKeyboard();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().szDebug = b;
		}

		CDebugInfo::SetDrawFPS(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iOutfitGuns = b;

			CWeaponsOutFit::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeRadarrect(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iRadarRect = b;

			CRadarRect::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeSkyBox(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iSkyBox = b;
			g_iStatusDriftChanged = 1;
		}
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeCutoutSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iCutout;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iAndroidKeyboard;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().szDebug;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHpArmourText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHPArmourText;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iOutfitGuns;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeRadarrect(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iRadarRect;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeSkyBox(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iSkyBox;
		}
		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowSave(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			pSettings->Save();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowDefaults(JNIEnv* pEnv, jobject thiz, jint category)
	{
		if (pSettings)
		{
			pSettings->ToDefaults(category);
			//CChatWindow::m_bPendingReInit = true;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id, jint a, jint r, jint g, jint b)
	{
		CAdjustableHudColors::SetHudColorFromRGBA((E_HUD_ELEMENT)id, r, g, b, a);
	}

	JNIEXPORT jbyteArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id)
	{
		char pTemp[9];
		jbyteArray color = pEnv->NewByteArray(sizeof(pTemp));

		if (!color)
		{
			return nullptr;
		}

		pEnv->SetByteArrayRegion(color, 0, sizeof(pTemp), (const jbyte*)CAdjustableHudColors::GetHudColorString((E_HUD_ELEMENT)id).c_str());

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		if (id == HUD_SNOW)
		{
			if (pSettings)
			{
				pSettings->GetWrite().iSnow = x;
			}
			CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);
			return;
		}
		CAdjustableHudPosition::SetElementPosition((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];
		arr[0] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).X;
		arr[1] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).Y;
		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)& arr[0]);

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y, jint scale)
	{
		if (id == 0)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonMicrophoneX = x;
				pSettings->GetWrite().fButtonMicrophoneY = y;
				pSettings->GetWrite().fButtonMicrophoneSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
				{
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetWidth(scale);
				}
			}
		}
		

		if (id == 2)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonCameraCycleX = x;
				pSettings->GetWrite().fButtonCameraCycleY = y;
				pSettings->GetWrite().fButtonCameraCycleSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
				{
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetWidth(scale);
				}
			}
		}
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];

		if (id == HUD_SNOW && pSettings)
		{
			arr[0] = CSnow::GetCurrentSnow();
			arr[1] = CSnow::GetCurrentSnow();
		}
		else
		{
			arr[0] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).X;
			arr[1] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).Y;
		}

		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)&arr[0]);

		return color;
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(3);

		if (!color)
		{
			return nullptr;
		}
		int arr[3] = { -1, -1, -1 };
		

		if (pSettings)
		{
			if (id == 0)
			{
				arr[0] = pSettings->GetWrite().fButtonMicrophoneX;
				arr[1] = pSettings->GetWrite().fButtonMicrophoneY;
				arr[2] = pSettings->GetWrite().fButtonMicrophoneSize;
			}
			if (id == 1)
			{
				arr[0] = pSettings->GetWrite().fButtonEnterPassengerX;
				arr[1] = pSettings->GetWrite().fButtonEnterPassengerY;
				arr[2] = pSettings->GetWrite().fButtonEnterPassengerSize;
			}
			if (id == 2)
			{
				arr[0] = pSettings->GetWrite().fButtonCameraCycleX;
				arr[1] = pSettings->GetWrite().fButtonCameraCycleY;
				arr[2] = pSettings->GetWrite().fButtonCameraCycleSize;
			}
		}
		

		pEnv->SetIntArrayRegion(color, 0, 3, (const jint*)& arr[0]);

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedEngineClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendChatCommand("/e");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedLightsClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendChatCommand("/light");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNotifyFirstClick(JNIEnv *pEnv, jobject thiz, jint actionId) {
		pNetGame->SendNotifyButtonPacket(actionId, 1);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNotifySecondClick(JNIEnv *pEnv, jobject thiz, jint actionId) {
		pNetGame->SendNotifyButtonPacket(actionId, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterSkinBackClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->RegisterSkinValue--;
		if (g_pJavaWrapper->RegisterSexMale == 1) // man
		{
			if (g_pJavaWrapper->RegisterSkinValue < 1)
			{
				g_pJavaWrapper->RegisterSkinValue = 10;
			}
		}
		else if (g_pJavaWrapper->RegisterSexMale == 2) // woman
		{
			if (g_pJavaWrapper->RegisterSkinValue < 1)
			{
				g_pJavaWrapper->RegisterSkinValue = 4;
			}
		}
		//CChatWindow::AddDebugMessage("chooseskinvalue: %d, chooseskinid: %d", g_pJavaWrapper->RegisterSkinValue, g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
		pNetGame->SendRegisterSkinPacket(g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterSkinNextClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->RegisterSkinValue++;
		if (g_pJavaWrapper->RegisterSexMale == 1) // man
		{
			if (g_pJavaWrapper->RegisterSkinValue > 10)
			{
				g_pJavaWrapper->RegisterSkinValue = 1;
			}
		}
		else if (g_pJavaWrapper->RegisterSexMale == 2) // woman
		{
			if (g_pJavaWrapper->RegisterSkinValue > 4)
			{
				g_pJavaWrapper->RegisterSkinValue = 1;
			}
		}
		//CChatWindow::AddDebugMessage("chooseskinvalue: %d, chooseskinid: %d", g_pJavaWrapper->RegisterSkinValue, g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
		pNetGame->SendRegisterSkinPacket(g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterChooseSkinClick(JNIEnv *pEnv, jobject thiz, jint choosesex) {
		g_pJavaWrapper->RegisterSexMale = choosesex;
		pGame->ToggleHUDElement(0, false);
		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		CCamera *pCamera = pGame->GetCamera();

		pNetGame->SendRegisterSkinPacket(g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));

		if(pPlayer->IsInVehicle())
			pPlayer->RemoveFromVehicleAndPutAt(-82.9753, 966.7605, 1597.9788);
		else
			pPlayer->TeleportTo(-82.9753, 966.7605, 1597.9788);

		pPlayer->ForceTargetRotation(90.0f);

		if (pPlayer && pCamera)
		{
			pPlayer->SetInterior(2);
			pCamera->SetPosition(-85.068267, 966.699584, 1598.421997, 0.0f, 0.0f, 0.0f);
			pCamera->LookAtPoint(-80.124114, 967.120971, 1597.807373, 2);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterClick(JNIEnv *pEnv, jobject thiz, jstring password, jstring mail, jint choosesex) {
		const char *inputPassword = pEnv->GetStringUTFChars(password, nullptr);
		const char *inputMail = pEnv->GetStringUTFChars(mail, nullptr);

		if(pNetGame) {
			pNetGame->SendRegisterPacket((char*)inputPassword, (char*)inputMail, choosesex, g_pJavaWrapper->RegisterSkinId);
		}

		pGame->ToggleHUDElement(0, false);

		Log("onRegisterPlayClick: inputPassword - %s, inputMail - %s, ChooseSex - %d", inputPassword, inputMail, choosesex);

		pEnv->ReleaseStringUTFChars(password, inputPassword);
		pEnv->ReleaseStringUTFChars(mail, inputMail);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onLoginClick(JNIEnv *pEnv, jobject thiz, jstring password) {
		const char *inputPassword = pEnv->GetStringUTFChars(password, nullptr);

		if(pNetGame) {
			pNetGame->SendLoginPacket(inputPassword);
		}

		Log("onAuthPlayClick: inputPassword - %s", inputPassword);

		pEnv->ReleaseStringUTFChars(password, inputPassword);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onChooseSpawnClick(JNIEnv *pEnv, jobject thiz, jint spawnid) {
		pNetGame->SendCustomPacket(253, 2, spawnid);
		Log("SendChooseSpawn: SpawnId - %d", spawnid);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSamwillHideGame(JNIEnv *pEnv, jobject thiz, jint samwillpacket) {
		pNetGame->SendCustomPacket(251, 20, samwillpacket);
		Log("onSamwillHideGame: Quantity - %d", samwillpacket);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onTargetNotifyClose(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideTargetNotify();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onOilFactoryGameClose(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 33, 1);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onArmyGameClose(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 45, 1);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onFuelStationClick(JNIEnv *pEnv, jobject thiz, jint fueltype, jint fuelliters) {
		pNetGame->SendCustomPacketFuelData(251, 39, fueltype, fuelliters);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onShopStoreClick(JNIEnv *pEnv, jobject thiz, jint buttonid) {
		pNetGame->SendCustomPacket(251, 42, buttonid);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onGunShopClick(JNIEnv *pEnv, jobject thiz, jint weaponid) {
		pNetGame->SendCustomPacket(251, 44, weaponid);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onClientCheckHASH(JNIEnv *pEnv, jobject thiz, jstring text) {
		const char *password = pEnv->GetStringUTFChars(text, nullptr);
		pNetGame->SendCheckClientPacket((char*)password);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnRightClick(JNIEnv *pEnv, jobject thiz, jint state) {
		pNetGame->SendSpeedTurnPacket(2, state);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnCenterClick(JNIEnv *pEnv, jobject thiz, jint state) {
		pNetGame->SendSpeedTurnPacket(1, state);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnLeftClick(JNIEnv *pEnv, jobject thiz, jint state) {
		pNetGame->SendSpeedTurnPacket(0, state);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onDeathInfoWait(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 48, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onDeathInfoClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 48, 1);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAuctionButtonClick(JNIEnv *pEnv, jobject thiz, jint btnid) {
		pNetGame->SendCustomPacket(251, 52, btnid);
	}
}

void CJavaWrapper::ShowAuctionManager(int itemId, int type, int price)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showAuctionManager, itemId, type, price);
}

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

void CJavaWrapper::ShowBusInfo(int time)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showBusInfo, time);
}

void CJavaWrapper::HideBusInfo()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideBusInfo);
}

void CJavaWrapper::ShowYernMoney()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showYernMoney);
}

void CJavaWrapper::HideYernMoney()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideYernMoney);
}

void CJavaWrapper::UpdateYearnMoney(int money)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_updateYearnMoney, money);
}

void CJavaWrapper::ShowUpdateTargetNotify(int type, char *text)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);
    env->CallVoidMethod(this->activity, this->s_showUpdateTargetNotify, type, jtext);
}

void CJavaWrapper::HideTargetNotify()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideTargetNotify);
}

void CJavaWrapper::ShowGreenZone()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showGreenZone);
}

void CJavaWrapper::HideGreenZone()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideGreenZone);
}

void CJavaWrapper::ShowGPS()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showGPS);
}

void CJavaWrapper::HideGPS()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideGPS);
}

void CJavaWrapper::UpdateLevelInfo(int level, int currentexp, int maxexp)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateLevelInfo, level, currentexp, maxexp);
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

void CJavaWrapper::HideSamwill()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideSamwill);
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

void CJavaWrapper::ShowSplash() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showSplash);
}

void CJavaWrapper::UpdateSplash(int percent) {
	
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateSplash, percent);
}


void CJavaWrapper::ShowServerLogo() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showServerLogo);
}

void CJavaWrapper::HideServerLogo() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_hideServerLogo);
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

void CJavaWrapper::ShowSpeed()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showSpeed);
}

void CJavaWrapper::HideSpeed()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideSpeed);
}

void CJavaWrapper::UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateSpeedInfo, speed, fuel, hp, mileage, engine, light, belt, lock);
}

void CJavaWrapper::ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn, int actionId) 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes, 0, strlen(text), (jbyte*)text);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	bytes = env->NewByteArray(strlen(actionforBtn));
    env->SetByteArrayRegion(bytes, 0, strlen(actionforBtn), (jbyte*)actionforBtn);
    jstring jactionforBtn = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	bytes = env->NewByteArray(strlen(textBtn));
    env->SetByteArrayRegion(bytes, 0, strlen(textBtn), (jbyte*)textBtn);
    jstring jtextBtn = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	env->CallVoidMethod(this->activity, this->s_showNotification, type, jtext, duration, jactionforBtn, jtextBtn, actionId);
}

void CJavaWrapper::HideNotification() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_hideNotification);
}

void CJavaWrapper::ShowDeathInfo(std::string nick, int id)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jstring jStringParam = env->NewStringUTF( nick.c_str() );

	env->CallVoidMethod(this->activity, this->j_showDeathInfo, jStringParam, id);

	pGame->isPreDeathActive = true;
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
void CJavaWrapper::UpdateAutoShop(const char name[], int price, int count, float maxspeed, float acceleration)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jstring j_name = env->NewStringUTF( name );

	env->CallVoidMethod(this->activity, this->j_updateAutoShop, j_name, price, count, maxspeed, acceleration);

	env->DeleteLocalRef(j_name);
}

void CJavaWrapper::HideDeathInfo() 
{
//	JNIEnv* env = GetEnv();
//
//	if (!env)
//	{
//		Log("No env");
//		return;
//	}
//	env->CallVoidMethod(this->activity, this->s_hideDeathInfo);
}

void CJavaWrapper::ShowRegistration(char *nick, int id) 
{
	pGame->isRegistrationActive = true;
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

	jbyteArray bytes = env->NewByteArray(strlen(nick));
    env->SetByteArrayRegion(bytes, 0, strlen(nick), (jbyte*)nick);
    jstring jnick = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	env->CallVoidMethod(this->activity, this->s_showRegistration, jnick, id);
	g_pJavaWrapper->RegisterSkinValue = 1;
}

void CJavaWrapper::HideRegistration() 
{
	pGame->isRegistrationActive = false;
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->s_hideRegistration);
}

void CJavaWrapper::ShowAuthorization(char *nick, int id, bool ip_match, bool toggleAutoLogin, bool email_acvive)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jstring jnick = env->NewStringUTF( nick );

	env->CallVoidMethod(this->activity, this->s_showAuthorization, jnick, id, ip_match, toggleAutoLogin, email_acvive);
}

void CJavaWrapper::HideAuthorization() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->s_hideAuthorization);
}

void CJavaWrapper::ShowChooseSpawn(int organization, int station, int exit, int garage, int house) 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->s_showChooseSpawn, organization, station, exit, garage, house);
}

void CJavaWrapper::HideChooseSpawn() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->s_hideChooseSpawn);
}

void CJavaWrapper::ClearScreen()
{
	Log("ClearScreen");
	ShowMiningGame1(false);
	ShowMiningGame2(false);
	ShowMiningGame3(false);
	ToggleShopStoreManager(false);
	HideTargetNotify();
	HideAuthorization();
	HideChooseSpawn();
	HideRegistration();
	HideSpeed();
	HideArmyGame();
	this->ToggleAutoShop(false);
	HideBusInfo();
	HideGPS();
	HideGreenZone();
	HideYernMoney();
	HideSamwill();
	ShowCasinoDice(false, 0, 0, 0, 0, "--", 0, "--", 0, "--", 0, "--", 0, "--", 0);
}

const uint32_t cRegisterSkin[2][10] = {
	{ 9, 195, 231, 232, 1, 1, 1, 1, 1, 1 }, // female
	{ 16, 79, 134, 135, 136, 200, 234, 235, 236, 239 } // male
};

uint32_t CJavaWrapper::ChangeRegisterSkin(int skin)
{
	uint32_t uiSkin = 16;
	bool bIsMan = g_pJavaWrapper->RegisterSexMale == 1 ? true : false;
	uint32_t uiMaxSkins = bIsMan ? 10 : 4;

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

	s_GetClipboardText = env->GetMethodID(nvEventClass, "getClipboardText", "()[B");

	s_ShowInputLayout = env->GetMethodID(nvEventClass, "showInputLayout", "()V");
	s_HideInputLayout = env->GetMethodID(nvEventClass, "hideInputLayout", "()V");

	s_ShowClientSettings = env->GetMethodID(nvEventClass, "showClientSettings", "()V");
	s_SetUseFullScreen = env->GetMethodID(nvEventClass, "setUseFullscreen", "(I)V");

	s_updateLevelInfo = env->GetMethodID(nvEventClass, "updateLevelInfo", "(III)V");

	s_showGreenZone = env->GetMethodID(nvEventClass, "showGreenZone", "()V");
    s_hideGreenZone = env->GetMethodID(nvEventClass, "hideGreenZone", "()V");
	s_showGPS = env->GetMethodID(nvEventClass, "showGPS", "()V");
    s_hideGPS = env->GetMethodID(nvEventClass, "hideGPS", "()V");

	s_showOilFactoryGame = env->GetMethodID(nvEventClass, "showOilFactoryGame", "()V");
	s_showArmyGame = env->GetMethodID(nvEventClass, "showArmyGame", "(I)V");
	s_hideArmyGame = env->GetMethodID(nvEventClass, "hideArmyGame", "()V");

	s_showFuelStation = env->GetMethodID(nvEventClass, "showFuelStation", "(IIIIIII)V");

	//s_showAuctionManager = env->GetMethodID(nvEventClass, "showAuctionManager", "(III)V");

	s_ToggleShopStoreManager = env->GetMethodID(nvEventClass, "toggleShopStoreManager", "(ZII)V");

	s_showGunShopManager = env->GetMethodID(nvEventClass, "showGunShopManager", "()V");
	s_hideGunShopManager = env->GetMethodID(nvEventClass, "hideGunShopManager", "()V");

	s_showBusInfo = env->GetMethodID(nvEventClass, "showBusInfo", "(I)V");
	s_hideBusInfo = env->GetMethodID(nvEventClass, "hideBusInfo", "()V");

	s_showYernMoney = env->GetMethodID(nvEventClass, "showYernMoney", "()V");
	s_hideYernMoney = env->GetMethodID(nvEventClass, "hideYernMoney", "()V");
	s_updateYearnMoney = env->GetMethodID(nvEventClass, "updateYearnMoney", "(I)V");
	s_showUpdateTargetNotify = env->GetMethodID(nvEventClass, "showUpdateTargetNotify", "(ILjava/lang/String;)V");
	s_hideTargetNotify = env->GetMethodID(nvEventClass, "hideTargetNotify", "()V");

    s_showSamwill = env->GetMethodID(nvEventClass, "showSamwill", "()V");
    s_hideSamwill = env->GetMethodID(nvEventClass, "hideSamwill", "()V");

	s_updateSpeedInfo = env->GetMethodID(nvEventClass, "updateSpeedInfo", "(IIIIIIII)V");
    s_showSpeed = env->GetMethodID(nvEventClass, "showSpeed", "()V");
    s_hideSpeed = env->GetMethodID(nvEventClass, "hideSpeed", "()V");
	
	s_showNotification = env->GetMethodID(nvEventClass, "showNotification","(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;I)V");
	s_hideNotification = env->GetMethodID(nvEventClass, "hideNotification", "()V");
	s_showMenu = env->GetMethodID(nvEventClass, "showMenu", "()V");

	s_showAuthorization = env->GetMethodID(nvEventClass, "showAuthorization", "(Ljava/lang/String;IZZZ)V");
	s_hideAuthorization = env->GetMethodID(nvEventClass, "hideAuthorization", "()V");

	s_showRegistration = env->GetMethodID(nvEventClass, "showRegistration", "(Ljava/lang/String;I)V");
	s_hideRegistration = env->GetMethodID(nvEventClass, "hideRegistration", "()V");

	j_showDeathInfo = env->GetMethodID(nvEventClass, "showPreDeath", "(Ljava/lang/String;I)V");
	//s_hideDeathInfo = env->GetMethodID(nvEventClass, "hideDeathInfo", "()V");

	j_toggleAutoShop = env->GetMethodID(nvEventClass, "toggleAutoShop", "(Z)V");

	j_updateAutoShop = env->GetMethodID(nvEventClass, "updateAutoShop", "(Ljava/lang/String;IIFF)V");

	s_showChooseSpawn = env->GetMethodID(nvEventClass, "showChooseSpawn", "(IIIII)V");
	s_hideChooseSpawn = env->GetMethodID(nvEventClass, "hideChooseSpawn", "()V");

	s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

	s_updateSplash = env->GetMethodID(nvEventClass, "updateSplash", "(I)V");
	s_showSplash = env->GetMethodID(nvEventClass, "showSplash", "()V");

	s_showServerLogo = env->GetMethodID(nvEventClass, "showServerLogo", "()V");
	s_hideServerLogo = env->GetMethodID(nvEventClass, "hideServerLogo", "()V");


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
}

void CJavaWrapper::ShowCasinoLuckyWheel(int count, int time) {

	pGame->isCasinoWheelActive = true;
	JNIEnv* env = GetEnv();

	jclass clazz = env->GetObjectClass(jCasino_LuckyWheel);
	jmethodID Show = env->GetMethodID(clazz, "Show", "(II)V");

	env->CallVoidMethod(jCasino_LuckyWheel, Show, count, time);
}

CJavaWrapper* g_pJavaWrapper = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_native_1SendAutoShopButton(JNIEnv *env, jobject thiz,
																		jint button_id) {
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
{
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_SendCasinoButt(JNIEnv *env, jobject thiz,
                                                            jint buttonID) {
    uint8_t packet = ID_CUSTOM_RPC;
    uint8_t RPC = RPC_SHOW_DICE_TABLE;
    uint8_t button = buttonID;


    RakNet::BitStream bsSend;
    bsSend.Write(packet);
    bsSend.Write(RPC);
    bsSend.Write(button);
    pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

}



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
Java_com_liverussia_cr_core_DialogClientSettingsCommonFragment_ChatFontSizeChanged(JNIEnv *env,
																				   jobject thiz,
																				   jint size) {
	if (pSettings)
	{
		pSettings->GetWrite().fFontSize = size;
		//CChatWindow::m_bPendingReInit = true;
	}
}
extern "C"
JNIEXPORT void JNICALL
Java_com_liverussia_cr_gui_PreDeath_OnClickPreDeathButton(JNIEnv *env, jobject thiz,
                                                          jint button_id) {
	pGame->isPreDeathActive = false;

	uint8_t packet = ID_CUSTOM_RPC;
	uint8_t RPC = RPC_PRE_DEATH;
	uint8_t button = button_id;


	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(button);
	pNetGame->GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);
}

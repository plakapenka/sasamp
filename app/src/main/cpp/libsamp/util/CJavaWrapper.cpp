#include "CJavaWrapper.h"
#include "../main.h"
#include <vector>

extern "C" JavaVM* javaVM;

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "..//net/netgame.h"
#include "../game/game.h"
#include "utf8_to_cp1251.h"

extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CGame* pGame;

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

void CJavaWrapper::CallLauncherActivity()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(activity, s_CallLauncherActivity);

	EXCEPTION_CHECK(env);
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

void CJavaWrapper::HideDialogWithoutReset()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideDialogWithoutReset);
}

void CJavaWrapper::MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText)
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
    jbyteArray bytes = env->NewByteArray(strlen(caption));
    env->SetByteArrayRegion(bytes, 0, strlen(caption), (jbyte*)caption);
    jstring str1 = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    //
    jclass strClass1 = env->FindClass("java/lang/String");
    jmethodID ctorID1 = env->GetMethodID(strClass1, "<init>", "([BLjava/lang/String;)V");
    jstring encoding1 = env->NewStringUTF("UTF-8");
    jbyteArray bytes1 = env->NewByteArray(strlen(content));
    env->SetByteArrayRegion(bytes1, 0, strlen(content), (jbyte*)content);
    jstring str2 = (jstring)env->NewObject(strClass1, ctorID1, bytes1, encoding1);
    //
    jclass strClass2 = env->FindClass("java/lang/String");
    jmethodID ctorID2 = env->GetMethodID(strClass2, "<init>", "([BLjava/lang/String;)V");
    jstring encoding2 = env->NewStringUTF("UTF-8");
    jbyteArray bytes2 = env->NewByteArray(strlen(leftBtnText));
    env->SetByteArrayRegion(bytes2, 0, strlen(leftBtnText), (jbyte*)leftBtnText);
    jstring str3 = (jstring)env->NewObject(strClass2, ctorID2, bytes2, encoding2);
    //
    jclass strClass3 = env->FindClass("java/lang/String");
    jmethodID ctorID3 = env->GetMethodID(strClass3, "<init>", "([BLjava/lang/String;)V");
    jstring encoding3 = env->NewStringUTF("UTF-8");
    jbyteArray bytes3 = env->NewByteArray(strlen(rightBtnText));
    env->SetByteArrayRegion(bytes3, 0, strlen(rightBtnText), (jbyte*)rightBtnText);
    jstring str4 = (jstring)env->NewObject(strClass3, ctorID3, bytes3, encoding3);

    env->CallVoidMethod(activity, s_MakeDialog, dialogId, dialogTypeId, str1, str2, str3, str4);

    EXCEPTION_CHECK(env);
	g_pJavaWrapper->isDialogActive = true;
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
extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInputEnd(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		if (pKeyBoard)
		{
			pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i3, jint i, jint i2, jstring str)
	{
		const char *inputText = pEnv->GetStringUTFChars(str, nullptr);
		char inputtext1[2048];
		// cp1251_to_utf8(inputtext1, inputText, 2048);
		convertUtf8ToCp1251(inputText, inputtext1);

		if(pNetGame) {
			pNetGame->SendDialogResponse(i, i3, i2, (char*)inputtext1);
			pGame->FindPlayerPed()->TogglePlayerControllable(true);
			g_pJavaWrapper->ShowHud();
			g_pJavaWrapper->ShowServer(pSettings->GetReadOnly().szServer);
		}

		g_pJavaWrapper->isDialogActive = false;

		Log("sendDialogResponse: inputtext1 - %s, inputText - %s", inputtext1, inputText);

		pEnv->ReleaseStringUTFChars(str, inputText);
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
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNativeHeightChanged(JNIEnv* pEnv, jobject thiz, jint orientation, jint height)
	{
		if (pChatWindow)
		{
			pChatWindow->SetLowerBound(height);
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

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeDebugCounterSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().szDebug = b;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHud(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iHud = b;
			if(!b)
			{
				*(uint8_t*)(g_libGTASA+0x7165E8) = 1;
				g_pJavaWrapper->HideHud();
			}
			else
			{
				*(uint8_t*)(g_libGTASA+0x7165E8) = 0;
				g_pJavaWrapper->ShowHud();
			}
			g_pJavaWrapper->ShowServer(pSettings->GetReadOnly().szServer);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeDialog(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iDialog = b;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHpArmourText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			if (!pSettings->GetWrite().iHPArmourText && b)
			{
				if (CAdjustableHudColors::IsUsingHudColor(HUD_HP_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_HP_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).X == -1 || CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).Y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_HP_TEXT, 500, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).X == -1 || CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).Y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_HP_TEXT, 400, 400);
				}

				if (CAdjustableHudColors::IsUsingHudColor(HUD_ARMOR_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_ARMOR_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).X == -1 || CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).Y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_ARMOR_TEXT, 300, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).X == -1 || CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).Y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_ARMOR_TEXT, 400, 400);
				}
			}

			pSettings->GetWrite().iHPArmourText = b;
		}

		CInfoBarText::SetEnabled(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iOutfitGuns = b;

			CWeaponsOutFit::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativePcMoney(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iPCMoney = b;
		}

		CGame::SetEnabledPCMoney(b);
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
	
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHud(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHud;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeDialog(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iDialog;
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

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeDebugCounterSettings(JNIEnv* pEnv, jobject thiz)
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

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativePcMoney(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iPCMoney;
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
			if (pChatWindow)
			{
				pChatWindow->m_bPendingReInit = true;
			}
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
		if (id == 7)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fChatPosX = x;
				pSettings->GetWrite().fChatPosY = y;
				if (pChatWindow)
				{
					pChatWindow->m_bPendingReInit = true;
				}
				return;
			}
			return;
		}
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
		CAdjustableHudPosition::SetElementPosition((HUD_RADAR), 97, 85);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)id, x, y);
		CAdjustableHudScale::SetElementScale((HUD_RADAR), 73, -1);

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

			//if (g_pWidgetManager)
			//{
			//	if (g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
			//	{
			//		g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetPos(x, y);
			//		g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetHeight(scale);
			//		g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetWidth(scale);
			//	}
			//}
		}
		
		if (id == 1)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonEnterPassengerX = x;
				pSettings->GetWrite().fButtonEnterPassengerY = y;
				pSettings->GetWrite().fButtonEnterPassengerSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_ENTEREXIT))
				{
					g_pWidgetManager->GetWidget(WIDGET_ENTEREXIT)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_ENTEREXIT)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_ENTEREXIT)->SetWidth(scale);
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

			//if (g_pWidgetManager)
			//{
			//	if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
			//	{
			//		g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetPos(x, y);
			//		g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetHeight(scale);
			//		g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetWidth(scale);
			//	}
			//}
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

		if (id == 7 && pSettings)
		{
			arr[0] = pSettings->GetReadOnly().fChatPosX;
			arr[1] = pSettings->GetReadOnly().fChatPosY;
		}
		else if (id == HUD_SNOW && pSettings)
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

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSamwillClicked(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->Samwillclicked = true;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNotifyEnterClick(JNIEnv *pEnv, jobject thiz) {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNotifyAltClick(JNIEnv *pEnv, jobject thiz) {
		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool)
		{
			CLocalPlayer *pLocalPlayer;
			if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
				LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
			else
				LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNotifyYClick(JNIEnv *pEnv, jobject thiz) {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNotifyNClick(JNIEnv *pEnv, jobject thiz) {
		LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
	}


	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoChipExitClick(JNIEnv *pEnv, jobject thiz, jint type) {
		pNetGame->SendCustomCasinoChipPacket(251, 34, type, 2, 0);
		g_pJavaWrapper->HideCasinoChip();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoChipButtonClick(JNIEnv *pEnv, jobject thiz, jint type, jint money) {
		pNetGame->SendCustomCasinoChipPacket(251, 34, type, 1, money);
		g_pJavaWrapper->HideCasinoChip();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoChipBackClick(JNIEnv *pEnv, jobject thiz, jint type) {
		pNetGame->SendCustomCasinoChipPacket(251, 34, type, 0, 0);
		g_pJavaWrapper->HideCasinoChip();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoRoulleteExitClick(JNIEnv *pEnv, jobject thiz, jint type) {
		g_pJavaWrapper->HideCasinoRoullete();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoRoulleteFreeClick(JNIEnv *pEnv, jobject thiz, jint type) {
		pNetGame->SendCustomPacket(251, 32, 0);
		g_pJavaWrapper->HideCasinoRoullete();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoRoulleteBuyClick(JNIEnv *pEnv, jobject thiz, jint type) {
		pNetGame->SendCustomPacket(251, 32, 1);
		g_pJavaWrapper->HideCasinoRoullete();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoSetBetClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 31, 0);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoDiceClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 31, 1);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onCasinoExitClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 31, 2);
		g_pJavaWrapper->HideCasino();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onZavodExitClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 28, 0);
		usleep(20000);
		g_pJavaWrapper->HideZavod();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onZavodSuccessful(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 28, 1);
		usleep(200000);
		g_pJavaWrapper->HideZavod();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onMiningExitClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 29, 0);
		usleep(20000);
		g_pJavaWrapper->HideMine();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onMiningSuccessful(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(251, 29, 1);
		usleep(200000);
		g_pJavaWrapper->HideMine();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickCamera(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 8);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickRight(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 7);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickExit(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 6);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickBuy(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 5);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickTestDrive(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 4);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickLeft(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 3);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickColorLeft(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 1);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAutosalonClickColorRight(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendCustomPacket(254, 3, 2);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedEngineClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendChatCommand("/e");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedLightsClick(JNIEnv *pEnv, jobject thiz) {
		pNetGame->SendChatCommand("/light");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onTargetNotifyClose(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideTargetNotify();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedLeftTurnClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->SpeedLeftTurn = true;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedRightTurnClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->SpeedRightTurn = true;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpeedTurnClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->SpeedTurn = true;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAuthClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ShowLoginTwo();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAuthBackClick(JNIEnv *pEnv, jobject thiz) {
		//g_pJavaWrapper->HideLoginTwo();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ShowRegisterTwo();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegBackClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideRegisterTwo();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onAuthPlayClick(JNIEnv* pEnv, jobject thiz, jstring password)
	{
		const char *inputText = pEnv->GetStringUTFChars(password, nullptr);

		if(pNetGame) {
			pNetGame->SendLoginPacket((char*)inputText);
		}

		Log("onAuthPlayClick: inputText - %s", inputText);

		pEnv->ReleaseStringUTFChars(password, inputText);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegPlayClick(JNIEnv* pEnv, jobject thiz)
	{
		g_pJavaWrapper->HideRegister();
		g_pJavaWrapper->ShowRegisterSex();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterSkinSendClick(JNIEnv *pEnv, jobject thiz, jstring mail, jstring password) {
		const char *inputTextMail = pEnv->GetStringUTFChars(mail, nullptr);
		const char *inputTextPassword = pEnv->GetStringUTFChars(password, nullptr);

		if(pNetGame) {
			pNetGame->SendRegisterPacket((char*)inputTextPassword, (char*)inputTextMail, g_pJavaWrapper->RegisterSexMale, g_pJavaWrapper->RegisterSkinId);
			g_pJavaWrapper->HideRegisterSkin();
		}

		Log("SendRegisterPacket: password - %s, mail - %s, sex - %d, skin - %d", inputTextPassword, inputTextMail, g_pJavaWrapper->RegisterSexMale, g_pJavaWrapper->RegisterSkinId);

		pEnv->ReleaseStringUTFChars(mail, inputTextMail);
		pEnv->ReleaseStringUTFChars(password, inputTextPassword);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterManClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->UpdateRegisterSex(1);
		g_pJavaWrapper->RegisterSexMale = 1;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterWomanClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->UpdateRegisterSex(2);
		g_pJavaWrapper->RegisterSexMale = 2;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterSendClick(JNIEnv *pEnv, jobject thiz) {
		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		CCamera *pCamera = pGame->GetCamera();
		if (pPlayer && pCamera)
		{
			pPlayer->SetInterior(2);
			pCamera->SetPosition(-85.068267, 966.699584, 1598.421997, 0.0f, 0.0f, 0.0f);
			pCamera->LookAtPoint(-80.124114, 967.120971, 1597.807373, 2);
		}
		g_pJavaWrapper->HideRegisterSex();
		g_pJavaWrapper->ShowRegisterSkin();
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{	
			uint16_t actorId;
			uint32_t iSkinId;
			VECTOR vecPos;
			actorId = 1;
			iSkinId = g_pJavaWrapper->ChangeRegisterSkin(1);
			vecPos.X = -82.9753;
			vecPos.Y = 966.7605;
			vecPos.Z = 1597.9788;
			pActorPool->Spawn(actorId, iSkinId, vecPos, 90.0f, 100.0f, true);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterSkinLeftClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->RegisterSkinValue--;
		if (g_pJavaWrapper->RegisterSexMale == 1) // man
		{
			if (g_pJavaWrapper->RegisterSkinValue < 1)
			{
				g_pJavaWrapper->RegisterSkinValue = 1;
			}
		}
		else if (g_pJavaWrapper->RegisterSexMale == 2) // woman
		{
			if (g_pJavaWrapper->RegisterSkinValue < 1)
			{
				g_pJavaWrapper->RegisterSkinValue = 1;
			}
		}
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{	
			CEntity* pActorEntity = (CEntity*)pNetGame->GetActorPool()->GetAt(1);
			pActorEntity->SetModelIndex(g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onRegisterSkinRightClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->RegisterSkinValue++;
		if (g_pJavaWrapper->RegisterSexMale == 1) // man
		{
			if (g_pJavaWrapper->RegisterSkinValue > 10)
			{
				g_pJavaWrapper->RegisterSkinValue = 10;
			}
		}
		else if (g_pJavaWrapper->RegisterSexMale == 2) // woman
		{
			if (g_pJavaWrapper->RegisterSkinValue > 4)
			{
				g_pJavaWrapper->RegisterSkinValue = 4;
			}
		}
		CActorPool* pActorPool = pNetGame->GetActorPool();
		if (pActorPool)
		{	
			CEntity* pActorEntity = (CEntity*)pNetGame->GetActorPool()->GetAt(1);
			pActorEntity->SetModelIndex(g_pJavaWrapper->ChangeRegisterSkin(g_pJavaWrapper->RegisterSkinValue));
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpawnFractionClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ChooseSpawn = 1;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpawnStationClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ChooseSpawn = 2;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpawnExitClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ChooseSpawn = 3;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpawnGarageClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ChooseSpawn = 4;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpawnHouseClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ChooseSpawn = 5;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSpawnSendClick(JNIEnv *pEnv, jobject thiz) {
		if (g_pJavaWrapper->ChooseSpawn != 0)
		{
			pNetGame->SendCustomPacket(253, 2, g_pJavaWrapper->ChooseSpawn);
			Log("SendChooseSpawn: SpawnId - %d", g_pJavaWrapper->ChooseSpawn);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudMenuClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->ShowMenu();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudMenuCloseClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudNavigatorClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/gps");
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudTaxiClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/call");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudMenuMenuClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/mm");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudInventoryClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/inv");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudAnimationClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/anim");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudDonateClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/donate");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudCarsClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/car");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudPromoClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/promo");
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onHudReportClick(JNIEnv *pEnv, jobject thiz) {
		g_pJavaWrapper->HideMenu();
		pNetGame->SendChatCommand("/report");
	}
}

void CJavaWrapper::ShowLogin(jstring nick, int id)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showLogin, nick, id);
}

void CJavaWrapper::HideLogin()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideLogin);
}

void CJavaWrapper::ShowLoginTwo()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showLoginTwo);
}

void CJavaWrapper::HideLoginTwo()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideLoginTwo);
}

void CJavaWrapper::ShowRegister()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showRegister);
}

void CJavaWrapper::HideRegister()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideRegister);
}

void CJavaWrapper::ShowRegisterTwo()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showRegisterTwo);
}

void CJavaWrapper::HideRegisterTwo()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideRegisterTwo);
}

void CJavaWrapper::ShowRegisterSex()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showRegisterSex);
	g_pJavaWrapper->HideHud();
	g_pJavaWrapper->RegisterSexMale = 0;
	g_pJavaWrapper->RegisterSkinValue = 1;
}

void CJavaWrapper::HideRegisterSex()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideRegisterSex);
}

void CJavaWrapper::ShowRegisterSkin()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showRegisterSkin);
	for (int i = 0; i < 7; i++)
	{
		pGame->ToggleHUDElement(i, false);
	}
}

void CJavaWrapper::HideRegisterSkin()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideRegisterSkin);
}

void CJavaWrapper::UpdateRegisterSex(int male)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_updateRegisterSex, male);
}

void CJavaWrapper::ShowSpawn(int organization, int station, int exit, int garage, int house)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showSpawn, organization, station, exit, garage, house);
	g_pJavaWrapper->ChooseSpawn = 0;
}

void CJavaWrapper::HideSpawn()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideSpawn);
}

void CJavaWrapper::HideLoadingScreen()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_hideLoadingScreen);
}

void CJavaWrapper::UpdateLoadingScreen(int procent)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_updateLoadingScreen, procent);
}

void CJavaWrapper::ShowKilllist()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_showKilllist);
}

void CJavaWrapper::HideKilllist()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideKilllist);
}

void CJavaWrapper::ShowKilllistMulti()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_showKilllistMulti);
}

void CJavaWrapper::HideKilllistMulti()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideKilllistMulti);
}

void CJavaWrapper::ShowKilllisDuels()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showKilllisDuels);
}

void CJavaWrapper::HideKilllistDuels()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideKilllistDuels);
}

void CJavaWrapper::UpdateKilllist(int kills, int deaths, int duelskills, int duelkillsmax)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_updateKilllist, kills, deaths, duelskills, duelkillsmax);
}

void CJavaWrapper::UpdateTargetNotify(int type, char *text)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass0 = env->FindClass("java/lang/String");
    jmethodID ctorID0 = env->GetMethodID(strClass0, "<init>", "([BLjava/lang/String;)V");
    jstring encoding0 = env->NewStringUTF("UTF-8");
    jbyteArray bytes0 = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytes0, 0, strlen(text), (jbyte*)text);
    jstring textstr = (jstring)env->NewObject(strClass0, ctorID0, bytes0, encoding0);
    //env->CallVoidMethod(this->activity, this->s_updateTargetNotify, type, textstr);
}


void CJavaWrapper::UpdateKilllistTop(char *one, char *two, char *three)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass11 = env->FindClass("java/lang/String");
    jmethodID ctorID11 = env->GetMethodID(strClass11, "<init>", "([BLjava/lang/String;)V");
    jstring encoding11 = env->NewStringUTF("UTF-8");
    jbyteArray bytes11 = env->NewByteArray(strlen(one));
    env->SetByteArrayRegion(bytes11, 0, strlen(one), (jbyte*)one);
    jstring onestr = (jstring)env->NewObject(strClass11, ctorID11, bytes11, encoding11);
	//
	jclass strClass22 = env->FindClass("java/lang/String");
    jmethodID ctorID22 = env->GetMethodID(strClass22, "<init>", "([BLjava/lang/String;)V");
    jstring encoding22 = env->NewStringUTF("UTF-8");
    jbyteArray bytes22 = env->NewByteArray(strlen(two));
    env->SetByteArrayRegion(bytes22, 0, strlen(two), (jbyte*)two);
    jstring twostr = (jstring)env->NewObject(strClass22, ctorID22, bytes22, encoding22);
	//
	jclass strClass33 = env->FindClass("java/lang/String");
    jmethodID ctorID33 = env->GetMethodID(strClass33, "<init>", "([BLjava/lang/String;)V");
    jstring encoding33 = env->NewStringUTF("UTF-8");
    jbyteArray bytes33 = env->NewByteArray(strlen(three));
    env->SetByteArrayRegion(bytes33, 0, strlen(three), (jbyte*)three);
    jstring threestr = (jstring)env->NewObject(strClass33, ctorID33, bytes33, encoding33);
   // env->CallVoidMethod(this->activity, this->s_updateKilllistTop, onestr, twostr, threestr);
}


void CJavaWrapper::UpdateKilllistOne(int team, int weapon, char *killer, char *killed)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass111 = env->FindClass("java/lang/String");
    jmethodID ctorID111 = env->GetMethodID(strClass111, "<init>", "([BLjava/lang/String;)V");
    jstring encoding111 = env->NewStringUTF("UTF-8");
    jbyteArray bytes111 = env->NewByteArray(strlen(killer));
    env->SetByteArrayRegion(bytes111, 0, strlen(killer), (jbyte*)killer);
    jstring killerstr = (jstring)env->NewObject(strClass111, ctorID111, bytes111, encoding111);
	//
	jclass strClass222 = env->FindClass("java/lang/String");
    jmethodID ctorID222 = env->GetMethodID(strClass222, "<init>", "([BLjava/lang/String;)V");
    jstring encoding222 = env->NewStringUTF("UTF-8");
    jbyteArray bytes222 = env->NewByteArray(strlen(killed));
    env->SetByteArrayRegion(bytes222, 0, strlen(killed), (jbyte*)killed);
    jstring killedstr = (jstring)env->NewObject(strClass222, ctorID222, bytes222, encoding222);
   // env->CallVoidMethod(this->activity, this->s_updateKilllistOne, team, weapon, killerstr, killedstr);
}

void CJavaWrapper::UpdateKilllistTwo(int team, int weapon, char *killer, char *killed)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass1111 = env->FindClass("java/lang/String");
    jmethodID ctorID1111 = env->GetMethodID(strClass1111, "<init>", "([BLjava/lang/String;)V");
    jstring encoding1111 = env->NewStringUTF("UTF-8");
    jbyteArray bytes1111 = env->NewByteArray(strlen(killer));
    env->SetByteArrayRegion(bytes1111, 0, strlen(killer), (jbyte*)killer);
    jstring killerstr = (jstring)env->NewObject(strClass1111, ctorID1111, bytes1111, encoding1111);
	//
	jclass strClass2222 = env->FindClass("java/lang/String");
    jmethodID ctorID2222 = env->GetMethodID(strClass2222, "<init>", "([BLjava/lang/String;)V");
    jstring encoding2222 = env->NewStringUTF("UTF-8");
    jbyteArray bytes2222 = env->NewByteArray(strlen(killed));
    env->SetByteArrayRegion(bytes2222, 0, strlen(killed), (jbyte*)killed);
    jstring killedstr = (jstring)env->NewObject(strClass2222, ctorID2222, bytes2222, encoding2222);
   // env->CallVoidMethod(this->activity, this->s_updateKilllistTwo, team, weapon, killerstr, killedstr);
}

void CJavaWrapper::UpdateKilllistThree(int team, int weapon, char *killer, char *killed)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass11111 = env->FindClass("java/lang/String");
    jmethodID ctorID11111 = env->GetMethodID(strClass11111, "<init>", "([BLjava/lang/String;)V");
    jstring encoding11111 = env->NewStringUTF("UTF-8");
    jbyteArray bytes11111 = env->NewByteArray(strlen(killer));
    env->SetByteArrayRegion(bytes11111, 0, strlen(killer), (jbyte*)killer);
    jstring killerstr = (jstring)env->NewObject(strClass11111, ctorID11111, bytes11111, encoding11111);
	//
	jclass strClass22222 = env->FindClass("java/lang/String");
    jmethodID ctorID22222 = env->GetMethodID(strClass22222, "<init>", "([BLjava/lang/String;)V");
    jstring encoding22222 = env->NewStringUTF("UTF-8");
    jbyteArray bytes22222 = env->NewByteArray(strlen(killed));
    env->SetByteArrayRegion(bytes22222, 0, strlen(killed), (jbyte*)killed);
    jstring killedstr = (jstring)env->NewObject(strClass22222, ctorID22222, bytes22222, encoding22222);
    //env->CallVoidMethod(this->activity, this->s_updateKilllistThree, team, weapon, killerstr, killedstr);
}

void CJavaWrapper::ClearKilllistDuels()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_clearKilllistDuels);
}

void CJavaWrapper::HideKilllistOne()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideKilllistOne);
	g_pJavaWrapper->ShowVoice();
}

void CJavaWrapper::HideKilllistTwo()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideKilllistTwo);
	g_pJavaWrapper->ShowVoice();
}

void CJavaWrapper::HideKilllistThree()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideKilllistThree);
	g_pJavaWrapper->ShowVoice();
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

void CJavaWrapper::UpdateAutosalon(char *vehname, int price, int quantity, int maxspeed, int overclocking)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClass111111 = env->FindClass("java/lang/String");
    jmethodID ctorID111111 = env->GetMethodID(strClass111111, "<init>", "([BLjava/lang/String;)V");
    jstring encoding111111 = env->NewStringUTF("UTF-8");
    jbyteArray bytes111111 = env->NewByteArray(strlen(vehname));
    env->SetByteArrayRegion(bytes111111, 0, strlen(vehname), (jbyte*)vehname);
    jstring vehnamestr = (jstring)env->NewObject(strClass111111, ctorID111111, bytes111111, encoding111111);

   // env->CallVoidMethod(this->activity, this->s_updateAutosalon, vehnamestr, price, quantity, maxspeed, overclocking);
}

void CJavaWrapper::ShowZavod(int type)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_showZavod, type);
}

void CJavaWrapper::HideZavod()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideZavod);
}

void CJavaWrapper::ShowMine(int type)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_showMine, type);
}

void CJavaWrapper::HideMine()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideMine);
}

void CJavaWrapper::ShowAutosalon()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_showAutosalon);
}

void CJavaWrapper::HideAutosalon()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_hideAutosalon);
}

void CJavaWrapper::UpdateNotification(int progress)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
  //  env->CallVoidMethod(this->activity, this->s_updateNotification, progress);
}

void CJavaWrapper::ShowNotification(int type, char* text)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strClasstext = env->FindClass("java/lang/String");
    jmethodID ctorIDtext = env->GetMethodID(strClasstext, "<init>", "([BLjava/lang/String;)V");
    jstring encodingtext = env->NewStringUTF("UTF-8");
    jbyteArray bytestext = env->NewByteArray(strlen(text));
    env->SetByteArrayRegion(bytestext, 0, strlen(text), (jbyte*)text);
    jstring textstr = (jstring)env->NewObject(strClasstext, ctorIDtext, bytestext, encodingtext);
   // env->CallVoidMethod(this->activity, this->s_showNotification, type, textstr);
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

void CJavaWrapper::ShowUpdateCasino(int tableid, int bet, int bank, int balance, char* playerid1, int playerstatus1, char* playerid2, int playerstatus2, char* playerid3, int playerstatus3, char* playerid4, int playerstatus4, char* playerid5, int playerstatus5)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	jclass strplayerid1 = env->FindClass("java/lang/String");
    jmethodID ctorplayerid1 = env->GetMethodID(strplayerid1, "<init>", "([BLjava/lang/String;)V");
    jstring encodingplayerid1 = env->NewStringUTF("UTF-8");
    jbyteArray bytesplayerid1 = env->NewByteArray(strlen(playerid1));
    env->SetByteArrayRegion(bytesplayerid1, 0, strlen(playerid1), (jbyte*)playerid1);
    jstring playerid1str = (jstring)env->NewObject(strplayerid1, ctorplayerid1, bytesplayerid1, encodingplayerid1);
	//
	jclass strplayerid2 = env->FindClass("java/lang/String");
    jmethodID ctorplayerid2 = env->GetMethodID(strplayerid2, "<init>", "([BLjava/lang/String;)V");
    jstring encodingplayerid2 = env->NewStringUTF("UTF-8");
    jbyteArray bytesplayerid2 = env->NewByteArray(strlen(playerid2));
    env->SetByteArrayRegion(bytesplayerid2, 0, strlen(playerid2), (jbyte*)playerid2);
    jstring playerid2str = (jstring)env->NewObject(strplayerid2, ctorplayerid2, bytesplayerid2, encodingplayerid2);
	//
	jclass strplayerid3 = env->FindClass("java/lang/String");
    jmethodID ctorplayerid3 = env->GetMethodID(strplayerid3, "<init>", "([BLjava/lang/String;)V");
    jstring encodingplayerid3 = env->NewStringUTF("UTF-8");
    jbyteArray bytesplayerid3 = env->NewByteArray(strlen(playerid3));
    env->SetByteArrayRegion(bytesplayerid3, 0, strlen(playerid3), (jbyte*)playerid3);
    jstring playerid3str = (jstring)env->NewObject(strplayerid3, ctorplayerid3, bytesplayerid3, encodingplayerid3);
	//
	jclass strplayerid4 = env->FindClass("java/lang/String");
    jmethodID ctorplayerid4 = env->GetMethodID(strplayerid4, "<init>", "([BLjava/lang/String;)V");
    jstring encodingplayerid4 = env->NewStringUTF("UTF-8");
    jbyteArray bytesplayerid4 = env->NewByteArray(strlen(playerid4));
    env->SetByteArrayRegion(bytesplayerid4, 0, strlen(playerid4), (jbyte*)playerid4);
    jstring playerid4str = (jstring)env->NewObject(strplayerid4, ctorplayerid4, bytesplayerid4, encodingplayerid4);
	//
	jclass strplayerid5 = env->FindClass("java/lang/String");
    jmethodID ctorplayerid5 = env->GetMethodID(strplayerid5, "<init>", "([BLjava/lang/String;)V");
    jstring encodingplayerid5 = env->NewStringUTF("UTF-8");
    jbyteArray bytesplayerid5 = env->NewByteArray(strlen(playerid5));
    env->SetByteArrayRegion(bytesplayerid5, 0, strlen(playerid5), (jbyte*)playerid5);
    jstring playerid5str = (jstring)env->NewObject(strplayerid5, ctorplayerid5, bytesplayerid5, encodingplayerid5);

   // env->CallVoidMethod(this->activity, this->s_showUpdateCasino, tableid, bet, bank, balance, playerid1str, playerstatus1, playerid2str, playerstatus2, playerid3str, playerstatus3, playerid4str, playerstatus4, playerid5str, playerstatus5);
}

void CJavaWrapper::HideCasino()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideCasino);
}

void CJavaWrapper::ShowUpdateBus(int timer)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showUpdateBus, timer);
}

void CJavaWrapper::HideBus()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideBus);
}


void CJavaWrapper::ShowCasinoRoullete(int quantity)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_showCasinoRoullete, quantity);
}

void CJavaWrapper::HideCasinoRoullete()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideCasinoRoullete);
}

void CJavaWrapper::ShowCasinoChip(int type, int balance)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showCasinoChip, type, balance);
}

void CJavaWrapper::HideCasinoChip()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideCasinoChip);
}

void CJavaWrapper::ShowHud()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showHud);
	g_pJavaWrapper->ShowVoice();
	g_pJavaWrapper->isHudToggle = true;
}

void CJavaWrapper::HideHud()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideHud);
	g_pJavaWrapper->isHudToggle = false;
}

void CJavaWrapper::ShowVoice()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_showVoice);
}

void CJavaWrapper::HideVoice()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideVoice);
}

void CJavaWrapper::ShowServer(int server)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showServer, server);
}

void CJavaWrapper::HideServer()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideServer);
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

void CJavaWrapper::ShowSamwillMoney()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_showSamwillMoney);
}

void CJavaWrapper::HideSamwillMoney()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideSamwillMoney);
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

	g_pJavaWrapper->HideHud();
	g_pJavaWrapper->HideServer();
	g_pJavaWrapper->HideSamwillMoney();
	for (int i = 0; i < 7; i++)
	{
		pGame->ToggleHUDElement(i, false);
	}
}

void CJavaWrapper::HideMenu()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideMenu);
	g_pJavaWrapper->ShowHud();
	g_pJavaWrapper->ShowServer(pSettings->GetReadOnly().szServer);
	for (int i = 0; i < 7; i++)
	{
		pGame->ToggleHUDElement(i, true);
	}
}

void CJavaWrapper::ShowSpeedometr()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_showSpeedometr);
}

void CJavaWrapper::HideSpeedometr()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
   // env->CallVoidMethod(this->activity, this->s_hideSpeedometr);
}

void CJavaWrapper::UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoclip, int money, int wanted)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	//env->CallVoidMethod(this->activity, this->s_updateHudInfo, health, armour, hunger, weaponid, ammo, ammoclip, money, wanted);
}

void CJavaWrapper::UpdateSamwill(int start, int progress, int check1, int check2, int check3, int check4, int check5)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	//env->CallVoidMethod(this->activity, this->s_updateSamwill, start, progress, check1, check2, check3, check4, check5);
}

void CJavaWrapper::UpdateRegisterSkinButtons(int btnidclicked)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	//env->CallVoidMethod(this->activity, this->s_updateRegisterSkinButtons, btnidclicked);
}

void CJavaWrapper::UpdateSamwillMoney(int money)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	//env->CallVoidMethod(this->activity, this->s_updateSamwillMoney, money);
}

void CJavaWrapper::UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateSpeedometr, speed, fuel, hp, mileage, engine, light, belt, lock);
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

	s_CallLauncherActivity = env->GetMethodID(nvEventClass, "callLauncherActivity", "()V");
	s_GetClipboardText = env->GetMethodID(nvEventClass, "getClipboardText", "()[B");

	s_ShowInputLayout = env->GetMethodID(nvEventClass, "showInputLayout", "()V");
	s_HideInputLayout = env->GetMethodID(nvEventClass, "hideInputLayout", "()V");

	s_ShowClientSettings = env->GetMethodID(nvEventClass, "showClientSettings", "()V");
	s_SetUseFullScreen = env->GetMethodID(nvEventClass, "setUseFullscreen", "(I)V");
	s_MakeDialog = env->GetMethodID(nvEventClass, "showDialog", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	s_hideDialogWithoutReset = env->GetMethodID(nvEventClass, "hideDialogWithoutReset", "()V");

	s_updateHudInfo = env->GetMethodID(nvEventClass, "UpdateHudInfo", "(IIIIIIII)V");
	//s_updateSamwill = env->GetMethodID(nvEventClass, "updateSamwill", "(IIIIIII)V");
	//s_updateSamwillMoney = env->GetMethodID(nvEventClass, "updateSamwillMoney", "(I)V");
	//s_updateRegisterSkinButtons = env->GetMethodID(nvEventClass, "updateRegisterSkinButtons", "(I)V");
	s_updateSpeedometr = env->GetMethodID(nvEventClass, "updateSpeedInfo", "(IIIIIIII)V");
	s_showLogin = env->GetMethodID(nvEventClass, "showAuthorization", "(Ljava/lang/String;I)V");
    s_hideLogin = env->GetMethodID(nvEventClass, "hideAuthorization", "()V");

	// s_showLoginTwo = env->GetMethodID(nvEventClass, "showLoginTwo", "()V");
    // s_hideLoginTwo = env->GetMethodID(nvEventClass, "hideLoginTwo", "()V");
	//s_showRegister = env->GetMethodID(nvEventClass, "showRegistration", "()V");
    s_hideRegister = env->GetMethodID(nvEventClass, "hideRegistration", "()V");
	// s_showRegisterTwo = env->GetMethodID(nvEventClass, "showRegisterTwo", "()V");
    // s_hideRegisterTwo = env->GetMethodID(nvEventClass, "hideRegisterTwo", "()V");
	// s_showRegisterSex = env->GetMethodID(nvEventClass, "showRegisterSex", "()V");
    // s_hideRegisterSex = env->GetMethodID(nvEventClass, "hideRegisterSex", "()V");
	// s_showRegisterSkin = env->GetMethodID(nvEventClass, "showRegisterSkin", "()V");
    // s_hideRegisterSkin = env->GetMethodID(nvEventClass, "hideRegisterSkin", "()V");
	// s_updateRegisterSex = env->GetMethodID(nvEventClass, "updateRegisterSex", "(I)V");
	s_showSpawn = env->GetMethodID(nvEventClass, "showChooseSpawn", "(IIIII)V");
    s_hideSpawn = env->GetMethodID(nvEventClass, "hideChooseSpawn", "()V");
	//s_hideLoadingScreen = env->GetMethodID(nvEventClass, "hideLoadingScreen", "()V");
	//s_updateLoadingScreen = env->GetMethodID(nvEventClass, "updateLoadingScreen", "(I)V");
	//s_clearKilllistDuels = env->GetMethodID(nvEventClass, "clearKilllistDuels", "()V");
	//s_hideKilllistOne = env->GetMethodID(nvEventClass, "hideKilllistOne", "()V");
	//s_hideKilllistThree = env->GetMethodID(nvEventClass, "hideKilllistThree", "()V");
//	s_hideKilllistTwo = env->GetMethodID(nvEventClass, "hideKilllistTwo", "()V");

	s_hideTargetNotify = env->GetMethodID(nvEventClass, "hideTargetNotify", "()V");
	s_showNotification = env->GetMethodID(nvEventClass, "showNotification", "(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;I)V");
	s_hideNotification = env->GetMethodID(nvEventClass, "hideNotification", "()V");
	//s_updateNotification = env->GetMethodID(nvEventClass, "updateNotification", "(I)V");

	s_showUpdateBus = env->GetMethodID(nvEventClass, "showBusInfo", "(I)V");
	s_hideBus = env->GetMethodID(nvEventClass, "hideBusInfo", "()V");


	//s_showCasinoRoullete = env->GetMethodID(nvEventClass, "showCasinoRoullete", "(I)V");
	//s_hideCasinoRoullete = env->GetMethodID(nvEventClass, "hideCasinoRoullete", "()V");

	//s_showCasinoChip = env->GetMethodID(nvEventClass, "showCasinoChip", "(II)V");
//	s_hideCasinoChip = env->GetMethodID(nvEventClass, "hideCasinoChip", "()V");

//	s_showUpdateCasino = env->GetMethodID(nvEventClass, "showUpdateCasino", "(IIIILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;ILjava/lang/String;I)V");
//	s_hideCasino = env->GetMethodID(nvEventClass, "hideCasino", "()V");


	//s_updateAutosalon = env->GetMethodID(nvEventClass, "updateAutosalon", "(Ljava/lang/String;IIII)V");
//	s_hideAutosalon = env->GetMethodID(nvEventClass, "hideAutosalon", "()V");
	//s_showAutosalon = env->GetMethodID(nvEventClass, "showAutosalon", "()V");


	//s_showZavod = env->GetMethodID(nvEventClass, "showZavod", "(I)V");
	//s_hideZavod = env->GetMethodID(nvEventClass, "hideZavod", "()V");
	//s_showMine = env->GetMethodID(nvEventClass, "showMine", "(I)V");
	//s_hideMine = env->GetMethodID(nvEventClass, "hideMine", "()V");

    s_showHud = env->GetMethodID(nvEventClass, "showHud", "()V");
    s_hideHud = env->GetMethodID(nvEventClass, "hideHud", "()V");
	//s_showVoice = env->GetMethodID(nvEventClass, "showVoice", "()V");
   // s_hideVoice = env->GetMethodID(nvEventClass, "hideVoice", "()V");
	s_showServer = env->GetMethodID(nvEventClass, "showServer", "(I)V");
    s_hideServer = env->GetMethodID(nvEventClass, "hideServer", "()V");
	s_showGPS = env->GetMethodID(nvEventClass, "showGPS", "()V");
    s_hideGPS = env->GetMethodID(nvEventClass, "hideGPS", "()V");
	s_showGreenZone = env->GetMethodID(nvEventClass, "showGreenZone", "()V");
    s_hideGreenZone = env->GetMethodID(nvEventClass, "hideGreenZone", "()V");
	s_showSamwill = env->GetMethodID(nvEventClass, "showSamwill", "()V");
    s_hideSamwill = env->GetMethodID(nvEventClass, "hideSamwill", "()V");
	//s_showSamwillMoney = env->GetMethodID(nvEventClass, "showSamwillMoney", "()V");
  //  s_hideSamwillMoney = env->GetMethodID(nvEventClass, "hideSamwillMoney", "()V");
	s_showMenu = env->GetMethodID(nvEventClass, "showMenu", "()V");
   // s_hideMenu = env->GetMethodID(nvEventClass, "hideMenu", "()V");
	//s_showSpeedometr = env->GetMethodID(nvEventClass, "showSpeedometr", "()V");
    //s_hideSpeedometr = env->GetMethodID(nvEventClass, "hideSpeedometr", "()V");
	//s_showKilllist = env->GetMethodID(nvEventClass, "showKilllist", "()V");
    //s_hideKilllist = env->GetMethodID(nvEventClass, "hideKilllist", "()V");
	//s_showKilllistMulti = env->GetMethodID(nvEventClass, "showKilllistMulti", "()V");
    //s_hideKilllistMulti = env->GetMethodID(nvEventClass, "hideKilllistMulti", "()V");
	//s_showKilllisDuels = env->GetMethodID(nvEventClass, "showKilllisDuels", "()V");
    //s_hideKilllistDuels = env->GetMethodID(nvEventClass, "hideKilllistDuels", "()V");
	//s_updateKilllist = env->GetMethodID(nvEventClass, "updateKilllist", "(IIII)V");
	//s_updateTargetNotify = env->GetMethodID(nvEventClass, "updateTargetNotify", "(ILjava/lang/String;)V");
	//s_updateKilllistTop = env->GetMethodID(nvEventClass, "updateKilllistTop", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
	//s_updateKilllistOne = env->GetMethodID(nvEventClass, "updateKilllistOne", "(IILjava/lang/String;Ljava/lang/String;)V");
	//s_updateKilllistTwo = env->GetMethodID(nvEventClass, "updateKilllistTwo", "(IILjava/lang/String;Ljava/lang/String;)V");
	//s_updateKilllistThree = env->GetMethodID(nvEventClass, "updateKilllistThree", "(IILjava/lang/String;Ljava/lang/String;)V");

	s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

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

CJavaWrapper* g_pJavaWrapper = nullptr;
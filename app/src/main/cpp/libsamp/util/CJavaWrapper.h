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
	jmethodID j_Vibrate;

	jmethodID s_ShowClientSettings;

	jmethodID s_showMenu;

	jmethodID s_setPauseState;

	jmethodID s_ExitGame;


public:
	JNIEnv* GetEnv();

	void ShowClientSettings();

	void ShowMenu();

	void SetPauseState(bool a1);

	CJavaWrapper(JNIEnv* env, jobject activity);
	~CJavaWrapper();

	void ClearScreen();

	void ExitGame();

	void Vibrate(int milliseconds);

    void SendBuffer(const char string[]);

	jobject activity;
};

extern CJavaWrapper* g_pJavaWrapper;
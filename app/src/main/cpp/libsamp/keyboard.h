#pragma once
#include "keyboardhistory.h"
#include "vendor/raknet/SingleProducerConsumer.h"
#include <jni.h>

#define LAYOUT_ENG 0
#define LAYOUT_RUS 1
#define LAYOUT_NUM 2

#define LOWER_CASE 0
#define UPPER_CASE 1

#define KEY_DEFAULT 0
#define KEY_SHIFT 1
#define KEY_BACKSPACE 2
#define KEY_SWITCH 3
#define KEY_SPACE 4
#define KEY_SEND 5
#define KEY_HISTORY 6
#define KEY_UP 7
#define KEY_DOWN 8

#define MAX_INPUT_LEN 255

struct kbKey
{
	ImVec2 pos;
	ImVec2 symPos;
	float width;
	char code[2];
	char name[2][4];
	int type;
	int id;
};

static class CKeyBoard
{
	friend class CGUI;

public:
	static void init();

	static void Open();
	static void Close();

	static bool IsOpen() { return m_bEnable; }
	static void AddCharToInput(char sym);
	static void Flush();

	static void EnableNewKeyboard();
	static void EnableOldKeyboard();

	static bool IsNewKeyboard();

	static std::vector<kbKey> m_Rows[3][4]; // eng, rus, num

	static std::string m_sInput;

	static int dop_butt;
public:
	static void Render();
	static bool OnTouchEvent(int type, bool multi, int x, int y);

	static void InitENG();
	static void InitRU();
	static void InitNUM();
	static kbKey *GetKeyFromPos(int x, int y);

	static void HandleInput(kbKey &key);
	static void DeleteCharFromInput();
	static void Send();

	static bool m_bEnable;

	static ImVec2 m_Size;
	static ImVec2 m_Pos;
	static float m_fKeySizeY;
	static float m_fKeySizeX;
	static float m_fFontSize;

	static bool m_iPushedKeyUp;
	static bool m_iPushedKeyDown;

	static ImFont *fonticon;
	static ImFont *chatfuncfont;

	static int m_iLayout;
	static int m_iCase;
	static int m_iPushedKey;
	static int chatinputposx;


	static char m_utf8Input[MAX_INPUT_LEN * 3 + 0xF];
	static CKeyBoardHistory *m_pkHistory;

	static DataStructures::SingleProducerConsumer<std::string> bufferedStrings;
};
#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>

#include "main.h"
#include "util/patch.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "gui/interface.h"
#include "chatwindow.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "CSettings.h"
#include "CClientInfo.h"
#include "scoreboard.h"
#include "CAudioStream.h"
#include "deathmessage.h"
#include "util/armhook.h"
#include "CCheckFileHash.h"
#include "str_obfuscator_no_template.hpp"

#include "game/materialtext.h"
#include "gui/CHUD.h"

#include "crashlytics.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

uintptr_t g_libGTASA = 0;
const char *g_pszStorage = nullptr;

#include "CServerManager.h"
#include "CLocalisation.h"

const cryptor::string_encryptor encLib = cryptor::create("libsamp.so", 11);
void CrashLog(const char *fmt, ...);
CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;
CSnapShotHelper *pSnapShotHelper = nullptr;
CScoreBoard *pScoreBoard = nullptr;
CAudioStream *pAudioStream = nullptr;
CDeathMessage *pDeathMessage = nullptr;
CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
// CJavaWrapper *pJavaWrapper      = nullptr;
CSettings *pSettings = nullptr;
CMaterialText *pMaterialText = nullptr;
CCrossHair *pCrossHair = nullptr;
CInterface *pInterface = nullptr;

void InitHookStuff();
void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

void PrintBuildInfo()
{
	Log("Build time: %s %s", __TIME__, __DATE__);
}

#ifdef GAME_EDITION_CR
extern uint16_t g_usLastProcessedModelIndexAutomobile;
extern int g_iLastProcessedModelIndexAutoEnt;
#endif

extern int g_iLastProcessedSkinCollision;
extern int g_iLastProcessedEntityCollision;

extern char g_bufRenderQueueCommand[200];
extern uintptr_t g_dwRenderQueueOffset;

void PrintBuildCrashInfo()
{
	CrashLog("Build time: %s %s", __TIME__, __DATE__);
}

#include <sstream>
#include "CClientInfo.h"
#include "vendor/bass/bass.h"
#include "gui/CFontRenderer.h"
#include "util/CJavaWrapper.h"
#include "cryptors/INITSAMP_result.h"
#include "CDebugInfo.h"
void InitSAMP(JNIEnv *pEnv, jobject thiz);
extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_initSAMP(JNIEnv *pEnv, jobject thiz)
	{
		InitSAMP(pEnv, thiz);
		//   pJavaWrapper = new CJavaWrapper(pEnv, thiz);
		//	pJavaWrapper->SetUseFullScreen(pSettings->Get().iCutout);
	}
}

void InitBASSFuncs();
void InitSAMP(JNIEnv *pEnv, jobject thiz)
{
	PROTECT_CODE_INITSAMP;

	Log("Initializing SAMP..");

	InitBASSFuncs();
	BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);

	g_pszStorage = /*(const char*)(g_libGTASA+0x63C4B8)*/ "/storage/emulated/0/Android/data/com.liverussia.cr/files/";

	if (!g_pszStorage)
	{
		Log("Error: storage path not found!");
		std::terminate();
		return;
	}

	PrintBuildInfo();

	Log("Storage: %s", g_pszStorage);

	pSettings = new CSettings();

	CWeaponsOutFit::SetEnabled(pSettings->GetReadOnly().iOutfitGuns);
	CRadarRect::SetEnabled(pSettings->GetReadOnly().iRadarRect);
	CGame::SetEnabledPCMoney(pSettings->GetReadOnly().iPCMoney);
	CInfoBarText::SetEnabled(pSettings->GetReadOnly().iHPArmourText);
	CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);

	g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);

	g_pJavaWrapper->SetUseFullScreen(pSettings->GetReadOnly().iCutout);

	CLocalisation::Initialise("ru.lc");

	firebase::crashlytics::SetUserId(pSettings->GetReadOnly().szNickName);

	CWeaponsOutFit::ParseDatFile();

	if (!CCheckFileHash::IsFilesValid())
	{
		// CClientInfo::bSAMPModified = false;
		return;
	}
}
extern CSnapShotHelper *pSnapShotHelper;
void ProcessCheckForKeyboard();
void GTASAInit();
void InitInMenu()
{
	GTASAInit();

	pGame = new CGame();
	pGame->InitInMenu();

	pGUI = new CGUI();
	pInterface = new CInterface();
	pKeyBoard = new CKeyBoard();
	pChatWindow = new CChatWindow();
	pPlayerTags = new CPlayerTags();
	pDialogWindow = new CDialogWindow();
	pScoreBoard = new CScoreBoard();
	pSnapShotHelper = new CSnapShotHelper();
	pAudioStream = new CAudioStream();
	pDeathMessage = new CDeathMessage();
	pMaterialText = new CMaterialText();
	pCrossHair = new CCrossHair();

	ProcessCheckForKeyboard();

	CFontRenderer::Initialise();
}
#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h>	// dlopen
bool unique_library_handler(const char *library)
{
	Dl_info info;
	if (dladdr(__builtin_return_address(0), &info) != 0)
	{
		void *current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
		if (!current_library_addr)
			return false;

		// Log("%p | %p", current_library_addr, dlopen(library, RTLD_NOW));

		if (dlopen(library, RTLD_NOW) != current_library_addr)
			return false;
	}
	return true;
}

void ProcessCheckForKeyboard()
{
	if (pSettings->GetReadOnly().iAndroidKeyboard)
	{
		pKeyBoard->EnableNewKeyboard();
	}
	else
	{
		pKeyBoard->EnableOldKeyboard();
	}
}

void ObfuscatedForceExit3()
{
	// CEntity::PreRender
}
#ifdef GAME_EDITION_CR
int g_iServer = 0;
#else
int g_iServer = 0;
#endif
void InitInGame()
{

	static bool bGameInited = false;
	static bool bNetworkInited = false;
	if (!bGameInited)
	{
		if (!unique_library_handler(encLib.decrypt()))
		{
			ObfuscatedForceExit3();
		}

		pGame->InitInGame();
		pGame->SetMaxStats();

		/*pDialogWindow->Clear();

		pDialogWindow->SetCurrentDialogId(0xFFF7);
		pDialogWindow->SetCurrentDialogStyle(DIALOG_STYLE_LIST);
		cp1251_to_utf8(pDialogWindow->m_utf8Title, "Select Server");

		cp1251_to_utf8(pDialogWindow->m_utf8Button1, "Select");

		cp1251_to_utf8(pDialogWindow->m_utf8Button2, "");


		char szBuff[400];
		memset(szBuff, 0, sizeof(szBuff));
		for (int i = 0; i < MAX_SERVERS; i++)
		{
			strcat(&szBuff[0], g_szServerNames[i]);
			strcat(&szBuff[0], "\n");
		}
		pDialogWindow->SetInfo(szBuff, strlen(szBuff));

		pDialogWindow->Show(true);*/

		// ����������� � �������
		pNetGame = new CNetGame(
			g_sEncryptedAddresses[pSettings->GetReadOnly().szServer - 1].decrypt(),
			g_sEncryptedAddresses[pSettings->GetReadOnly().szServer - 1].getPort(),
			pSettings->GetReadOnly().szNickName,
			pSettings->GetReadOnly().szPassword);

		bGameInited = true;

		return;
	}

	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint32_t dwColorYellow = 0x9008E8DE;
		uint32_t dwColorGreen = 0x9000ff00;
		pGangZonePool->New(1, 1960.60, 1353.00, 2150.70, 1465.35, dwColorYellow);
		pGangZonePool->New(2, 350.00, -2519.15, 595.50, -2304.50, dwColorYellow);
		// pGangZonePool->New(3, 1036.12, 1860.87, 2045.35, 1495.17, dwColorGreen);
	}

	if (!bNetworkInited)
	{
		// HERE IS
		bNetworkInited = true;
		return;
	}
}

void (*CTimer__StartUserPause)();
void CTimer__StartUserPause_hook()
{
	// process pause event
	if (g_pJavaWrapper)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsNewKeyboard())
			{
				pKeyBoard->Close();
			}
		}

		g_pJavaWrapper->SetPauseState(true);
	}

	*(uint8_t *)(g_libGTASA + 0x008C9BA3) = 1;
}

void (*CTimer__EndUserPause)();
void CTimer__EndUserPause_hook()
{
	// process resume event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->SetPauseState(false);
	}

	*(uint8_t *)(g_libGTASA + 0x008C9BA3) = 0;
}

#include "CDebugInfo.h"
void MainLoop()
{
	InitInGame();

	if (pNetGame)
		pNetGame->Process();

	if (pNetGame)
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CVehicle *pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					VECTOR vec;
					pVeh->GetMoveSpeedVector(&vec);
					CDebugInfo::ProcessSpeedMode(&vec);
				}
			}
		}
	}
	else
	{
		if (pGame)
		{
			pGame->SetWorldTime(12, 0);
			pGame->DisplayWidgets(false);
		}
	}
}
extern int g_iLastRenderedObject;
extern int V15lastest;
void PrintSymbols(void *pc, void *lr)
{
	CrashLog("PIPELINE: %d - 0x%X", V15lastest, V15lastest);
	Dl_info info_pc, info_lr;
	if (dladdr(pc, &info_pc) != 0)
	{
		CrashLog("PC: %s", info_pc.dli_sname);
	}
	if (dladdr(lr, &info_lr) != 0)
	{
		CrashLog("LR: %s", info_lr.dli_sname);
	}
	CrashLog("PIPELINE: %d - 0x%X", V15lastest, V15lastest);
}

void printAddressBacktrace(const unsigned address, void *pc, void *lr)
{
	char filename[0xFF];
	sprintf(filename, "/proc/%d/maps", getpid());
	FILE *m_fp = fopen(filename, "rt");
	if (m_fp == nullptr)
	{
		CrashLog("ERROR: can't open file %s", filename);
		return;
	}
	Dl_info info_pc, info_lr;
	memset(&info_pc, 0, sizeof(Dl_info));
	memset(&info_lr, 0, sizeof(Dl_info));
	dladdr(pc, &info_pc);
	dladdr(lr, &info_lr);

	rewind(m_fp);
	char buffer[2048] = {0};
	while (fgets(buffer, sizeof(buffer), m_fp))
	{
		const auto start_address = strtoul(buffer, nullptr, 16);
		const auto end_address = strtoul(strchr(buffer, '-') + 1, nullptr, 16);

		if (start_address <= address && end_address > address)
		{
			if (*(strchr(buffer, ' ') + 3) == 'x')
				CrashLog("Call: %X (GTA: %X PC: %s LR: %s) (SAMP: %X) (libc: %X)", address, address - g_libGTASA, info_pc.dli_sname, info_lr.dli_sname, address - FindLibrary("libsamp.so"), address - FindLibrary("libc.so"));
			break;
		}
	}
}

struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;

void handler3(int signum, siginfo_t *info, void *contextPtr)
{
	ucontext *context = (ucontext_t *)contextPtr;

	if (act3_old.sa_sigaction)
	{
		act3_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGBUS)
	{
		int crashId = (int)rand() % 20000;
		Log("Crashed. handler3 - %d", crashId);
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID: %d", crashId);
		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("SIGBUS | Fault address: 0x%X", info->si_addr);
		CrashLog("libGTASA base address: 0x%X", g_libGTASA);
		CrashLog("libsamp base address: 0x%X", FindLibrary("libsamp.so"));
		CrashLog("libc base address: 0x%X", FindLibrary("libc.so"));
		CrashLog("register states:");
		CrashLog("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X",
				 context->uc_mcontext.arm_r0,
				 context->uc_mcontext.arm_r1,
				 context->uc_mcontext.arm_r2,
				 context->uc_mcontext.arm_r3);
		CrashLog("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x",
				 context->uc_mcontext.arm_r4,
				 context->uc_mcontext.arm_r5,
				 context->uc_mcontext.arm_r6,
				 context->uc_mcontext.arm_r7);
		CrashLog("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x",
				 context->uc_mcontext.arm_r8,
				 context->uc_mcontext.arm_r9,
				 context->uc_mcontext.arm_r10,
				 context->uc_mcontext.arm_fp);
		CrashLog("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x",
				 context->uc_mcontext.arm_ip,
				 context->uc_mcontext.arm_sp,
				 context->uc_mcontext.arm_lr,
				 context->uc_mcontext.arm_pc);

		CrashLog("backtrace:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		CrashLog("1: libc.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
		CrashLog("2: libc.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libc.so"));

		PrintSymbols((void *)(context->uc_mcontext.arm_pc), (void *)(context->uc_mcontext.arm_lr));

		CrashLog("=======START STACKTRACE=======");
		for (auto i = 0; i < 100; ++i) // сколько хочешь, у меня 1000
		{
			const auto address = *reinterpret_cast<uintptr_t *>(context->uc_mcontext.arm_sp + 4 * i);

			printAddressBacktrace(address, (void *)(context->uc_mcontext.arm_pc + 4 * i), (void *)(context->uc_mcontext.arm_lr + 4 * i));
		}

		CrashLog("========END STACKTRACE========");

		// DumpLibraries();

		exit(0);
	}

	return;
}

void handler(int signum, siginfo_t *info, void *contextPtr)
{
	ucontext *context = (ucontext_t *)contextPtr;

	if (act_old.sa_sigaction)
	{
		act_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGSEGV)
	{

		int crashId = (int)rand() % 20000;
		Log("Crashed. handler - %d", crashId);
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID: %d", crashId);
		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("SIGSEGV | Fault address: 0x%X", info->si_addr);
		CrashLog("libGTASA base address: 0x%X", g_libGTASA);
		CrashLog("libsamp base address: 0x%X", FindLibrary("libsamp.so"));
		CrashLog("libc base address: 0x%X", FindLibrary("libc.so"));
		CrashLog("register states:");
		CrashLog("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X",
				 context->uc_mcontext.arm_r0,
				 context->uc_mcontext.arm_r1,
				 context->uc_mcontext.arm_r2,
				 context->uc_mcontext.arm_r3);
		CrashLog("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x",
				 context->uc_mcontext.arm_r4,
				 context->uc_mcontext.arm_r5,
				 context->uc_mcontext.arm_r6,
				 context->uc_mcontext.arm_r7);
		CrashLog("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x",
				 context->uc_mcontext.arm_r8,
				 context->uc_mcontext.arm_r9,
				 context->uc_mcontext.arm_r10,
				 context->uc_mcontext.arm_fp);
		CrashLog("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x",
				 context->uc_mcontext.arm_ip,
				 context->uc_mcontext.arm_sp,
				 context->uc_mcontext.arm_lr,
				 context->uc_mcontext.arm_pc);

		CrashLog("backtrace:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		CrashLog("1: libc.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
		CrashLog("2: libc.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libc.so"));

		PrintSymbols((void *)(context->uc_mcontext.arm_pc), (void *)(context->uc_mcontext.arm_lr));

		CrashLog("=======START STACKTRACE=======");
		for (auto i = 0; i < 100; ++i) // сколько хочешь, у меня 1000
		{
			const auto address = *reinterpret_cast<uintptr_t *>(context->uc_mcontext.arm_sp + 4 * i);

			printAddressBacktrace(address, (void *)(context->uc_mcontext.arm_pc + 4 * i), (void *)(context->uc_mcontext.arm_lr + 4 * i));
		}

		CrashLog("========END STACKTRACE========");

		// DumpLibraries();

		exit(0);
	}

	return;
}

void handler2(int signum, siginfo_t *info, void *contextPtr)
{
	ucontext *context = (ucontext_t *)contextPtr;

	if (act2_old.sa_sigaction)
	{
		act2_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGFPE)
	{

		int crashId = (int)rand() % 20000;
		Log("Crashed. handler2 - %d", crashId);
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID: %d", crashId);
		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("SIGFPE | Fault address: 0x%X", info->si_addr);
		CrashLog("libGTASA base address: 0x%X", g_libGTASA);
		CrashLog("libsamp base address: 0x%X", FindLibrary("libsamp.so"));
		CrashLog("libc base address: 0x%X", FindLibrary("libc.so"));
		CrashLog("register states:");
		CrashLog("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X",
				 context->uc_mcontext.arm_r0,
				 context->uc_mcontext.arm_r1,
				 context->uc_mcontext.arm_r2,
				 context->uc_mcontext.arm_r3);
		CrashLog("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x",
				 context->uc_mcontext.arm_r4,
				 context->uc_mcontext.arm_r5,
				 context->uc_mcontext.arm_r6,
				 context->uc_mcontext.arm_r7);
		CrashLog("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x",
				 context->uc_mcontext.arm_r8,
				 context->uc_mcontext.arm_r9,
				 context->uc_mcontext.arm_r10,
				 context->uc_mcontext.arm_fp);
		CrashLog("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x",
				 context->uc_mcontext.arm_ip,
				 context->uc_mcontext.arm_sp,
				 context->uc_mcontext.arm_lr,
				 context->uc_mcontext.arm_pc);

		CrashLog("backtrace:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		CrashLog("1: libc.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
		CrashLog("2: libc.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libc.so"));

		PrintSymbols((void *)(context->uc_mcontext.arm_pc), (void *)(context->uc_mcontext.arm_lr));

		CrashLog("=======START STACKTRACE=======");
		for (auto i = 0; i < 100; ++i) // сколько хочешь, у меня 1000
		{
			const auto address = *reinterpret_cast<uintptr_t *>(context->uc_mcontext.arm_sp + 4 * i);

			printAddressBacktrace(address, (void *)(context->uc_mcontext.arm_pc + 4 * i), (void *)(context->uc_mcontext.arm_lr + 4 * i));
		}

		CrashLog("========END STACKTRACE========");

		// DumpLibraries();

		exit(0);
	}

	return;
}

void handler1(int signum, siginfo_t *info, void *contextPtr)
{
	ucontext *context = (ucontext_t *)contextPtr;

	if (act1_old.sa_sigaction)
	{
		act1_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGABRT)
	{

		int crashId = (int)rand() % 20000;
		Log("Crashed. handler1 - %d", crashId);
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("ID: %d", crashId);

		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("SIGABRT | Fault address: 0x%X", info->si_addr);
		CrashLog("libGTASA base address: 0x%X", g_libGTASA);
		CrashLog("libsamp base address: 0x%X", FindLibrary("libsamp.so"));
		CrashLog("libc base address: 0x%X", FindLibrary("libc.so"));
		CrashLog("register states:");
		CrashLog("r0: 0x%X, r1: 0x%X, r2: 0x%X, r3: 0x%X",
				 context->uc_mcontext.arm_r0,
				 context->uc_mcontext.arm_r1,
				 context->uc_mcontext.arm_r2,
				 context->uc_mcontext.arm_r3);
		CrashLog("r4: 0x%x, r5: 0x%x, r6: 0x%x, r7: 0x%x",
				 context->uc_mcontext.arm_r4,
				 context->uc_mcontext.arm_r5,
				 context->uc_mcontext.arm_r6,
				 context->uc_mcontext.arm_r7);
		CrashLog("r8: 0x%x, r9: 0x%x, sl: 0x%x, fp: 0x%x",
				 context->uc_mcontext.arm_r8,
				 context->uc_mcontext.arm_r9,
				 context->uc_mcontext.arm_r10,
				 context->uc_mcontext.arm_fp);
		CrashLog("ip: 0x%x, sp: 0x%x, lr: 0x%x, pc: 0x%x",
				 context->uc_mcontext.arm_ip,
				 context->uc_mcontext.arm_sp,
				 context->uc_mcontext.arm_lr,
				 context->uc_mcontext.arm_pc);

		CrashLog("backtrace:");
		CrashLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_libGTASA);
		CrashLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_libGTASA);

		PrintSymbols((void *)(context->uc_mcontext.arm_pc), (void *)(context->uc_mcontext.arm_lr));

		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		CrashLog("1: libc.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
		CrashLog("2: libc.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libc.so"));

		CrashLog("=======START STACKTRACE=======");
		for (auto i = 0; i < 100; ++i) // сколько хочешь, у меня 1000
		{
			const auto address = *reinterpret_cast<uintptr_t *>(context->uc_mcontext.arm_sp + 4 * i);

			printAddressBacktrace(address, (void *)(context->uc_mcontext.arm_pc + 4 * i), (void *)(context->uc_mcontext.arm_lr + 4 * i));
		}

		CrashLog("========END STACKTRACE========");

		// DumpLibraries();

		exit(0);
	}

	return;
}

extern "C"
{
	JavaVM *javaVM = NULL;
	JavaVM *alcGetJavaVM(void)
	{
		return javaVM;
	}
}

void (*RQ_Command_rqSetAlphaTest)(char **);
void RQ_Command_rqSetAlphaTest_hook(char **a1)
{
	return;
}

#include "CFPSFix.h"
CFPSFix g_fps;

void (*ANDRunThread)(void *a1);
void ANDRunThread_hook(void *a1)
{
	g_fps.PushThread(gettid());

	ANDRunThread(a1);
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	javaVM = vm;

	Log("SAMP library loaded! Build time: " __DATE__ " " __TIME__);

	g_libGTASA = FindLibrary("libGTASA.so");
	if (g_libGTASA == 0)
	{
		Log("ERROR: libGTASA.so address not found!");
		return 0;
	}
	Log("libGTASA.so image base address: 0x%X", g_libGTASA);

	firebase::crashlytics::Initialize();

	uintptr_t libgtasa = FindLibrary("libGTASA.so");
	uintptr_t libsamp = FindLibrary("libsamp.so");
	uintptr_t libc = FindLibrary("libc.so");

	Log("libGTASA.so: 0x%x", libgtasa);
	Log("libsamp.so: 0x%x", libsamp);
	Log("libc.so: 0x%x", libc);

	char str[100];

	sprintf(str, "0x%x", libgtasa);
	firebase::crashlytics::SetCustomKey("libGTASA.so", str);

	sprintf(str, "0x%x", libsamp);
	firebase::crashlytics::SetCustomKey("libsamp.so", str);

	sprintf(str, "0x%x", libc);
	firebase::crashlytics::SetCustomKey("libc.so", str);

	srand(time(0));

	CPatch::InitHookStuff();

	InitRenderWareFunctions();
	InstallSpecialHooks();
	CPatch::SetUpHook(g_libGTASA + 0x003BF784, &CTimer__StartUserPause_hook, &CTimer__StartUserPause);
	CPatch::SetUpHook(g_libGTASA + 0x003BF7A0, &CTimer__EndUserPause_hook, &CTimer__EndUserPause);
	WriteMemory(g_libGTASA + 0x001BDD4A, (uintptr_t) "\x10\x46\xA2\xF1\x04\x0B", 6);
	WriteMemory(g_libGTASA + 0x003E1A2C, (uintptr_t) "\x67\xE0", 2);
	CPatch::SetUpHook(g_libGTASA + 0x0023768C, &ANDRunThread_hook, &ANDRunThread);
	// increase render memory buffer
	WriteMemory(g_libGTASA + 0x001A7EF2, (uintptr_t) "\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);
	WriteMemory(g_libGTASA + 0x001A7F34, (uintptr_t) "\x4F\xF4\x40\x10\x4F\xF4\x40\x10", 8);

	// yes, just nop-out this fucking shit
	// this should prevent game from crashing when exiting(R*)
	NOP(g_libGTASA + 0x0039844E, 2);
	NOP(g_libGTASA + 0x0039845E, 2);
	NOP(g_libGTASA + 0x0039840A, 2);

	NOP(g_libGTASA + 0x002E1EDC, 2); // get the fuck up this uninitialised shit!
	NOP(g_libGTASA + 0x00398972, 2); // get out fucking roadblocks
	// maybe nop engine terminating ????
	// terminate all stuff when exiting
	// nop shit pause

	if (!*(uintptr_t *)(g_libGTASA + 0x61B298))
	{
		uintptr_t test = ((uintptr_t(*)(const char *))(g_libGTASA + 0x00179A20))("glAlphaFuncQCOM");
		if (!test)
		{
			NOP(g_libGTASA + 0x001A6164, 4);
			CPatch::SetUpHook(g_libGTASA + 0x001A6164, &RQ_Command_rqSetAlphaTest_hook, &RQ_Command_rqSetAlphaTest);
		}
	}

	ApplyPatches_level0();

	struct sigaction act;
	act.sa_sigaction = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &act, &act_old);

	struct sigaction act1;
	act1.sa_sigaction = handler1;
	sigemptyset(&act1.sa_mask);
	act1.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &act1, &act1_old);

	struct sigaction act2;
	act2.sa_sigaction = handler2;
	sigemptyset(&act2.sa_mask);
	act2.sa_flags = SA_SIGINFO;
	sigaction(SIGFPE, &act2, &act2_old);

	struct sigaction act3;
	act3.sa_sigaction = handler3;
	sigemptyset(&act3.sa_mask);
	act3.sa_flags = SA_SIGINFO;
	sigaction(SIGBUS, &act3, &act3_old);

	return JNI_VERSION_1_4;
}

void Log(const char *fmt, ...)
{
	char buffer[0xFFFD];
	static FILE *flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/samp_log.txt", g_pszStorage);
		flLog = fopen(buffer, "w");
	}
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	// firebase::crashlytics::Log(buffer);

	// if(pDebug) pDebug->AddMessage(buffer);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	if (flLog == nullptr)
		return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

void CrashLog(const char *fmt, ...)
{
	char buffer[0xFF];
	static FILE *flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/crash_log.log", g_pszStorage);
		flLog = fopen(buffer, "a+");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	firebase::crashlytics::Log(buffer);

	if (flLog == nullptr)
		return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}


#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>

#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "playertags.h"
#include "keyboard.h"
#include "CSettings.h"
#include "CClientInfo.h"
#include "java_systems/CHUD.h"
#include "CLoader.h"

#include "CCheckFileHash.h"
#include "str_obfuscator_no_template.hpp"

#include "crashlytics.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

uintptr_t g_libGTASA = 0;
char g_pszStorage[255];

#include "CServerManager.h"
#include "CLocalisation.h"
#include "java_systems/CHUD.h"
#include "java_systems/CInventory.h"

const cryptor::string_encryptor encLib = cryptor::create("libsamp.so", 11);
void CrashLog(const char* fmt, ...);
bool isTestMode = false;
CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CPlayerTags *pPlayerTags = nullptr;

CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;

void InstallSpecialHooks();
void InitRenderWareFunctions();
void ApplyInGamePatches();
void ApplyPatches_level0();
void MainLoop();

void PrintBuildInfo()
{
	Log("SA:MP version: %d.%01d", SAMP_MAJOR_VERSION, SAMP_MINOR_VERSION);
	Log("Build times: %s %s", __TIME__, __DATE__);
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
	CrashLog("SA:MP version: %d.%01d", SAMP_MAJOR_VERSION, SAMP_MINOR_VERSION);
	CrashLog("Build times: %s %s", __TIME__, __DATE__);
	CrashLog("Last processed auto and entity: %d %d", g_usLastProcessedModelIndexAutomobile, g_iLastProcessedModelIndexAutoEnt);
	CrashLog("Last processed skin and entity: %d %d", g_iLastProcessedSkinCollision, g_iLastProcessedEntityCollision);
}

#include <sstream>
#include "CClientInfo.h"
#include "vendor/bass/bass.h"
#include "gui/CFontRenderer.h"
#include "util/CJavaWrapper.h"
#include "cryptors/INITSAMP_result.h"
#include "CDebugInfo.h"

void InitSAMP(JNIEnv* pEnv, jobject thiz, const char path[])
{
	Log("Initializing SAMP..");

	CLoader::redirectDirection(path);

	CSettings::LoadSettings(nullptr);

	isTestMode = (bool)CSettings::m_Settings.isTestMode;

	g_pJavaWrapper = new CJavaWrapper(pEnv, thiz);

	CLoader::initCrashLytics();

	CWeaponsOutFit::ParseDatFile();
}

void ProcessCheckForKeyboard();

void InitInMenu()
{
	pGame = new CGame();

	pGame->InitInMenu();

	pGUI = new CGUI();
	pKeyBoard = new CKeyBoard();
	pPlayerTags = new CPlayerTags();

	ProcessCheckForKeyboard();

	CFontRenderer::Initialise();
}
#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen
bool unique_library_handler(const char* library)
{
	Dl_info info;
	if (dladdr(__builtin_return_address(0), &info) != 0)
	{
		void* current_library_addr = dlopen(info.dli_fname, RTLD_NOW);
		if (!current_library_addr)
			return false;

		//Log("%p | %p", current_library_addr, dlopen(library, RTLD_NOW));

		if (dlopen(library, RTLD_NOW) != current_library_addr)
			return false;
	}
	return true;
}

void ProcessCheckForKeyboard()
{
	if (CSettings::m_Settings.iAndroidKeyboard)
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
int g_iServer = 1;
#else
int g_iServer = 1;
#endif
#include "net/CUDPSocket.h"
void InitInGame()
{

	//CAdjustableHudPosition::SetElementPosition((HUD_RADAR), 150, 150); //97, 85
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

		g_pJavaWrapper->UpdateSplash(101);

		g_pJavaWrapper->ShowServerLogo();
		bGameInited = true;

		return;
	}

	if (!bNetworkInited)
	{
		const cryptor::string_encryptor keytokey = cryptor::create("sadfjnrRnfmplaka1", 17);

		CChatWindow::AddDebugMessage("{bbbbbb}Клиент {ff0000}LIVE RUSSIA{bbbbbb} запущен");
		pNetGame = new CNetGame(
			g_sEncryptedAddresses[CSettings::m_Settings.szServer].decrypt(),
			g_sEncryptedAddresses[CSettings::m_Settings.szServer].getPort(),
			CSettings::m_Settings.szNickName,
			keytokey.decrypt());

		bNetworkInited = true;
		Log("InitInGame() end");
		return;
	}
}

#include "CDebugInfo.h"
void MainLoop()
{
	InitInGame();

	if(pNetGame) pNetGame->Process();
}
extern int g_iLastRenderedObject;
char g_iLastBlock[512];

void PrintSymbols(void* pc, void* lr)
{
	Dl_info info_pc, info_lr;
	if (dladdr(pc, &info_pc) != 0)
	{
		CrashLog("PC: %s", info_pc.dli_sname);
	}
	if (dladdr(lr, &info_lr) != 0)
	{
		CrashLog("LR: %s", info_lr.dli_sname);
	}
}


struct sigaction act_old;
struct sigaction act1_old;
struct sigaction act2_old;
struct sigaction act3_old;

void handler3(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act3_old.sa_sigaction)
	{
		act3_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGBUS)
	{
		PrintBuildCrashInfo();
		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("Last texture: %s", g_iLastBlock);
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

		//DumpLibraries();

		exit(0);
	}

	return;
}

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act_old.sa_sigaction)
	{
		act_old.sa_sigaction(signum, info, contextPtr);
	}

	if(info->si_signo == SIGSEGV)
	{
		CrashLog(" ");
		PrintBuildCrashInfo();
		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("Last texture: %s", g_iLastBlock);
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

		//DumpLibraries();

		exit(0);
	}

	return;
}

void handler2(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act2_old.sa_sigaction)
	{
		act2_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGFPE)
	{
		PrintBuildCrashInfo();
		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("Last texture: %s", g_iLastBlock);
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

		//DumpLibraries();

		exit(0);
	}

	return;
}

void handler1(int signum, siginfo_t* info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if (act1_old.sa_sigaction)
	{
		act1_old.sa_sigaction(signum, info, contextPtr);
	}

	if (info->si_signo == SIGABRT)
	{
		CrashLog(" ");
		PrintBuildCrashInfo();

		CrashLog("Last rendered object: %d", g_iLastRenderedObject);
		CrashLog("Last texture: %s", g_iLastBlock);
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


		CrashLog("1: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libsamp.so"));
		CrashLog("2: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libsamp.so"));

		CrashLog("1: libc.so + 0x%X", context->uc_mcontext.arm_pc - FindLibrary("libc.so"));
		CrashLog("2: libc.so + 0x%X", context->uc_mcontext.arm_lr - FindLibrary("libc.so"));


		//DumpLibraries();

		exit(0);
	}

	return;
}

extern "C"
{
	JavaVM* javaVM = NULL;
	JavaVM* alcGetJavaVM(void) {
		return javaVM;
	}
}

#include "CFPSFix.h"
#include "util/patch.h"
#include "CLoader.h"

CFPSFix g_fps;

void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
	g_fps.PushThread(gettid());

	ANDRunThread(a1);
}



jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	javaVM = vm;

	g_libGTASA = FindLibrary("libGTASA.so");
	if(g_libGTASA == 0)
	{
		Log("ERROR: libGTASA.so address not found!");
		return 0;
	}
	Log("libGTASA.so image base address: 0x%X", g_libGTASA);

	CHook::InitHookStuff();

	InstallSpecialHooks();
	InitRenderWareFunctions();

	ApplyPatches_level0();

	CLoader::loadBassLib();

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

	return JNI_VERSION_1_6;
}

void Log(const char *fmt, ...)
{	
	char buffer[512];

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	firebase::crashlytics::Log(buffer);
	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	//if(pDebug) pDebug->AddMessage(buffer);
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/samp_log.txt", g_pszStorage);
		flLog = fopen(buffer, "a");
	}

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

void CrashLog(const char* fmt, ...)
{
	char buffer[512];
	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	firebase::crashlytics::Log(buffer);
	__android_log_write(ANDROID_LOG_FATAL, "AXL", buffer);

	static FILE* flLog = nullptr;

	if (flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/crash_log.log", g_pszStorage);
		flLog = fopen(buffer, "a");
	}

	if (flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_iniGameDir(JNIEnv *env, jobject thiz, jstring path) {
	Log("// TODO: implement iniGameDir()");
}
extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_initSAMP(JNIEnv *env, jobject thiz,
													  jstring game_path) {
	const char *path = env->GetStringUTFChars(game_path, nullptr);

	InitSAMP(env, thiz, path);

	env->ReleaseStringUTFChars(game_path, path);
}
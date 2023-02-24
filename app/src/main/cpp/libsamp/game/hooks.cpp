#include "../main.h"
#include "RW/RenderWare.h"
#include <sstream>
#include "game.h"
#include "ePedState.h"

#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../util/CJavaWrapper.h"
#include "../java_systems/CHUD.h"
#include "..///..//santrope-tea-gtasa/encryption/CTinyEncrypt.h"
#include "..///..//santrope-tea-gtasa/encryption/encrypt.h"
#include "CGtaWidgets.h"
extern "C"
{
#include "..//santrope-tea-gtasa/encryption/aes.h"
}

#include "..///..//santrope-tea-gtasa/CGameResourcesDecryptor.cpp"
#include "..///..//santrope-tea-gtasa/CGameResourcesDecryptor.h"
#include "chatwindow.h"
#include "../util/patch.h"
#include "java_systems/CSpeedometr.h"

extern CGame* pGame;
extern CPlayerPed *g_pCurrentFiredPed;
extern BULLET_DATA *g_pCurrentBulletData;

static uint32_t dwRLEDecompressSourceSize = 0;


#include "..//str_obfuscator_no_template.hpp"
#define MAX_ENCRYPTED_TXD 3
const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = {
        cryptor::create("texdb/txd/txd.txt", 19),
        cryptor::create("texdb/gta3/gta3.txt", 21),
        cryptor::create("texdb/gta_int/gta_int.txt", 27),
};

extern bool g_bIsTestMode;

bool isEncrypted(const char *szArch)
{
    if(g_bIsTestMode)return false;
	//return false;
    for (int i = 0; i < MAX_ENCRYPTED_TXD; i++)
    {
        if (!strcmp(encrArch[i].decrypt(), szArch))
            return true;
    }
    return false;
}
void InitCTX(AES_ctx& ctx, const uint8_t* pKey)
{
    uint8_t key[16];
    memcpy(&key[0], &pKey[0], 16);
    for (int i = 0; i < 16; i++)
    {
        key[i] = XOR_UNOBFUSCATE(key[i]);
    }
    uint8_t iv[16];
    memcpy(&iv[0], &g_iIV, 16);
    for (int i = 0; i < 16; i++)
    {
        iv[i] = XOR_UNOBFUSCATE(iv[i]);
    }
    AES_init_ctx_iv(&ctx, &key[0], &iv[0]);
}

int lastOpenedFile;

extern CNetGame *pNetGame;
extern CGUI *pGUI;
// Neiae/SAMP
bool g_bPlaySAMP = false;

void InitInMenu();
void MainLoop();
void HookCPad();

/* ================ ie?oee aey ani. anoaaie =================== */

extern "C" uintptr_t get_lib() 
{
 	return g_libGTASA;
}

/* ====================================================== */

struct stFile
{
	int isFileExist;
	FILE *f;
};

stFile* (*NvFOpen)(const char*, const char*, int, int);
stFile* NvFOpen_hook(const char* r0, const char* r1, int r2, int r3)
{
	//return NvFOpen(r0, r1, r2, r3);

	char path[0xFF] = { 0 };

	sprintf(path, "%s%s", g_pszStorage, r1);

//	// ----------------------------
	if(!strncmp(r1+12, "mainV1.scm", 10))
	{
		sprintf(path, "%sSAMP/main.scm", g_pszStorage);
		Log("Loading %s", path);
	}
	// ----------------------------
	if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sSAMP/script.img", g_pszStorage);
		Log("Loading script.img..");
	}
//	// ----------------------------
	if(!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%sSAMP/peds.ide", g_pszStorage);
		Log("Loading peds.ide..");
	}
//	// ----------------------------
	if(!strncmp(r1, "DATA/VEHICLES.IDE", 17))
	{
		sprintf(path, "%sSAMP/vehicles.ide", g_pszStorage);
		Log("Loading vehicles.ide..");
	}

	if (!strncmp(r1, "DATA/GTA.DAT", 12))
	{
		sprintf(path, "%sSAMP/gta.dat", g_pszStorage);
		Log("Loading gta.dat..");
	}

	if (!strncmp(r1, "DATA/HANDLING.CFG", 17))
	{
		sprintf(path, "%sSAMP/handling.cfg", g_pszStorage);
		Log("Loading handling.cfg..");
	}

	if (!strncmp(r1, "DATA/WEAPON.DAT", 15))
	{
		sprintf(path, "%sSAMP/weapon.dat", g_pszStorage);
		Log("Loading weapon.dat..");
	}

	stFile *st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE *f  = fopen(path, "rb");
	if(f)
	{
		Log("NvFOpen_hook %s ", path );
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		st = nullptr;
		return nullptr;
	}
}

/* ====================================================== */
bool bGameStarted = false;
uint32_t bProcessedCleanStreamPool = 0;

void (*Render2dStuff)();
void Render2dStuff_hook()
{
   // Log("Render2dStuff_hook");
	if (CHUD::bIsShow) CHUD::UpdateHudInfo();

	bGameStarted = true;
//	MAKE_PROFILE(test, test_time);
	MainLoop();

//	LOG_PROFILE(test, test_time);
	if (pGUI) pGUI->Render();
	Render2dStuff();
}

// OsArray<FlowScreen::MenuItem>::Add
void (*MenuItem_add)(int r0, uintptr_t r1);
void MenuItem_add_hook(int r0, uintptr_t r1)
{
	static bool bMenuInited = false;
	char* name = *(char**)(r1+4);

	// убирает кнопки "Start Game" и "Stats" в меню
	if(g_bPlaySAMP && (
		!strcmp(name, "FEP_STG") ||
		!strcmp(name, "FEH_STA") ||
		!strcmp(name, "FEH_BRI") ))
		return;

	MenuItem_add(r0, r1);
}

/* ====================================================== */

// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook() {
	Log("InitialiseRenderWare ..");

	CHook::NOP(g_libGTASA + 0x0046F576, 2);
	CHook::NOP(g_libGTASA + 0x0046F588, 2); // mobile
	CHook::NOP(g_libGTASA + 0x0046F592, 2); // txd
	CHook::NOP(g_libGTASA + 0x0046F59C, 2); // gta3
	CHook::NOP(g_libGTASA + 0x0046F5A6, 2); // gta_int
	//CHook::NOP(g_libGTASA + 0x0046F5B0, 2); // cutscene
	//CHook::NOP(g_libGTASA + 0x0046F5C6, 2); // player
	CHook::NOP(g_libGTASA + 0x0046F5D2, 2); // menu

	TextureDatabaseRuntime::Load("samp", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);

	TextureDatabaseRuntime::Load("mobile", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("txd", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("gta3", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("gta_int", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("menu", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_PVR);

	//skins
//	TextureDatabase* skins = TextureDatabaseRuntime::Load("skins", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
//	TextureDatabaseRuntime::Register(skins);
//
//	// cars
//	TextureDatabase* cars = TextureDatabaseRuntime::Load("cars", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
//	TextureDatabaseRuntime::Register(cars);

	InitialiseRenderWare();
}

/* ====================================================== */
void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
//	RwCamera *camera = *(RwCamera **)(g_libGTASA + 0x95B064);
//
//	if (RwCameraBeginUpdate(camera))
//	{
//		DefinedState2d();
//		((void (*)())(g_libGTASA + 0x5519C8 + 1))(); // CSprite2d::InitPerFrame()
//		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)rwTEXTUREADDRESSCLAMP);
//		((void (*)(bool))(g_libGTASA + 0x198010 + 1))(false); // emu_GammaSet()

		const float percent = *(float*)(g_libGTASA + 0x009920C0)*2;
		if (percent <= 0.0f) return;

		g_pJavaWrapper->UpdateSplash((int)percent);

//		RwCameraEndUpdate(camera);
//		RwCameraShowRaster(camera, 0, 0);
//	}
}

int bBlockCWidgetRegionLookUpdate = 0;

/* ====================================================== */
#include "..//keyboard.h"

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	if (*(bool*)(g_libGTASA + 0x0096B514))
	{
		return TouchEvent(type, num, posX, posY);
	}

	if (g_pWidgetManager)
	{
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_UP, type, num, posX, posY);
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_DOWN, type, num, posX, posY);
	}
	bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);
	if (!bRet)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	if (CKeyBoard::IsOpen())
	{
		if (posX >= io.DisplaySize.x - ImGui::GetFontSize() * 4 && posY >= io.DisplaySize.y / 2 - (ImGui::GetFontSize() * 2.5) * 3 && posY <= io.DisplaySize.y / 2) // keys
		{
			return;
		}
	}

	if(bRet) 
		return TouchEvent(type, num, posX, posY);
}

/* ====================================================== */

void (*CStreaming_InitImageList)();
void CStreaming_InitImageList_hook()
{
	char* ms_files = (char*)(g_libGTASA + 0x00792D80);

	ms_files[0] = 0;
	*(uint32_t*)&ms_files[44] = 0;
	ms_files[48] = 0;
	*(uint32_t*)&ms_files[92] = 0;
	ms_files[96] = 0;
	*(uint32_t*)&ms_files[140] = 0;
	ms_files[144] = 0;
	*(uint32_t*)&ms_files[188] = 0;
	ms_files[192] = 0;
	*(uint32_t*)&ms_files[236] = 0;
	ms_files[240] = 0;
	*(uint32_t*)&ms_files[284] = 0;
	ms_files[288] = 0;
	*(uint32_t*)&ms_files[332] = 0;
	ms_files[336] = 0;
	*(uint32_t*)&ms_files[380] = 0;

	CStreaming::AddImageToList("TEXDB\\GTA3.IMG", true);
	//CStreaming::AddImageToList("TEXDB\\SKINS.IMG", true);
   // CStreaming::AddImageToList("TEXDB\\CARS.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SAMPCOL.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SAMP.IMG", true);
}

/* ====================================================== */

#include "CModelInfo.h"


/* ====================================================== */

uint32_t (*CRadar__GetRadarTraceColor)(uint32_t color, uint8_t bright, uint8_t friendly);
uint32_t CRadar__GetRadarTraceColor_hook(uint32_t color, uint8_t bright, uint8_t friendly)
{
	return TranslateColorCodeToRGBA(color);
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char *name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char *name)
{
	if (pNetGame && !strncmp(name, "CODEWAY", 7))
	{
		float findZ = ((float (*)(float, float))(g_libGTASA + 0x0042A7C4 + 1))(X, Y);
		findZ += 1.5f;

		Log("OnPlayerClickMap: %f, %f, %f", X, Y, Z);
		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(findZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
	bGZ = v1;
	if (pNetGame && pNetGame->GetGangZonePool()) 
		pNetGame->GetGangZonePool()->Draw();
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_ololo()
{
	if(pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp( ((dwParam1-(g_libGTASA+0x70E264))/0x20) );
	}
}

__attribute__((naked)) void PickupPickUp_hook()
{
	//LOGI("PickupPickUp_hook");

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
					"push {r0}\n\t"
					"blx get_lib\n\t"
					"add r0, #0x2D0000\n\t"
					"add r0, #0x009A00\n\t"
					"add r0, #1\n\t"
					"mov r1, r0\n\t"
					"pop {r0}\n\t"
					"pop {lr}\n\t"
					"push {r1}\n\t");
	
	// 
	__asm__ volatile("push {r0-r11, lr}\n\t"
					"mov %0, r4" : "=r" (dwParam1));

	__asm__ volatile("blx pickup_ololo\n\t");


	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
					"sub.w r2, r1, #0xD\n\t"
					"sub.w r2, r1, #8\n\t"
					"cmp r1, #6\n\t"
					"pop {pc}\n\t");
}
extern bool g_customFire;
// fire weapon hooks
uint32_t (*CWeapon__FireInstantHit)(CWeaponGta* thiz, CPedGta * pFiringEntity, CVector * vecOrigin, CVector * muzzlePosn, CEntityGta * targetEntity, CVector * target, CVector * originForDriveBy, int arg6, int muzzle);
uint32_t CWeapon__FireInstantHit_hook(CWeaponGta* thiz, CPedGta * pFiringEntity, CVector * vecOrigin, CVector * muzzlePosn, CEntityGta * targetEntity, CVector * target, CVector * originForDriveBy, int arg6, int muzzle)
{

	if(!g_customFire)
	{
		CPedGta *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
		if(pLocalPed)
		{
			if(pFiringEntity != pLocalPed)
				return muzzle;

			if(pNetGame)
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if(pPlayerPool)
					pPlayerPool->ApplyCollisionChecking();
			}

			if(pGame)
			{
				CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
				if(pPlayerPed)
					pPlayerPed->FireInstant();
			}

			if(pNetGame)
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if(pPlayerPool)
					pPlayerPool->ResetCollisionChecking();
			}

			return muzzle;
		}
	}
	g_customFire = false;
	return CWeapon__FireInstantHit(thiz, pFiringEntity, vecOrigin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}

void (*MainMenuScreen__Update)(uintptr_t* thiz, float deltaTime);
void MainMenuScreen__Update_hook(uintptr_t* thiz, float deltaTime)
{
	MainMenuScreen__Update(thiz, deltaTime);

	if(g_bPlaySAMP) return;

	InitInMenu();

	// StartGameScreen::OnNewGameCheck()
	(( void (*)())(g_libGTASA + 0x002A7270 + 1))();

	//*(uint32_t*)(g_libGTASA + 0x009E75B8) = 8;

	g_bPlaySAMP = true;
}

static char szLastBufferedName[40];
int (*cHandlingDataMgr__FindExactWord)(uintptr_t *thiz, char* line, char* nameTable, int entrySize, int entryCount);
int cHandlingDataMgr__FindExactWord_hook(uintptr_t *thiz, char* line, char* nameTable, int entrySize, int entryCount)
{
	strncpy(&szLastBufferedName[0], line, entrySize);
	return cHandlingDataMgr__FindExactWord(thiz, line, nameTable, entrySize, entryCount);
}

void (*cHandlingDataMgr__ConvertDataToGameUnits)(uintptr_t thiz, tHandlingData* handling);
void cHandlingDataMgr__ConvertDataToGameUnits_hook(uintptr_t thiz, tHandlingData* handling)
{
	int iHandling = ((int(*)(uintptr_t, char*))(g_libGTASA + 0x00570D28 + 1))(thiz, &szLastBufferedName[0]);

	CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);

	return cHandlingDataMgr__ConvertDataToGameUnits(thiz, handling);
}


#include "..//nv_event.h"
int32_t(*NVEventGetNextEvent_hooked)(NVEvent* ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent* ev, int waitMSecs)
{
	int32_t ret = NVEventGetNextEvent_hooked(ev, waitMSecs);

	if (ret)
	{
		if (ev->m_type == NV_EVENT_MULTITOUCH)
		{
			// process manually
			ev->m_type = (NVEventType)228;
		}

	}

	NVEvent event;
	NVEventGetNextEvent(&event);

	if (event.m_type == NV_EVENT_MULTITOUCH)
	{
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		if (type == NV_MULTITOUCH_CANCEL)
		{
			type = NV_MULTITOUCH_UP;
		}

		if ((x1 || y1) || num == 0)
		{
			if (num == 0 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 0, x1, y1); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
			}
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 1, x2, y2); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
			}
		}
		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 2, x3, y3); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
			}
		}
	}

	return ret;
}

void(*CStreaming__Init2)();
void CStreaming__Init2_hook()
{
	CStreaming__Init2();
	CHook::UnFuck(g_libGTASA + 0x00685FA0);
	*(uint32_t*)(g_libGTASA + 0x00685FA0) = 0x20000000;
}

signed int (*OS_FileOpen)(unsigned int a1, int *a2, const char *a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int *a2, const char *a3, int a4)
{
	uintptr_t calledFrom = 0;
	__asm__ volatile("mov %0, lr"
			: "=r"(calledFrom));
	calledFrom -= g_libGTASA;
	signed int retn = OS_FileOpen(a1, a2, a3, a4);

	//if (calledFrom == 0x1BCE9A + 1)
	//{
	if (isEncrypted(a3))
	{
		lastOpenedFile = *a2;
	}
	//  }
	return retn;
}

size_t (*OS_FileRead)(FILE *file, void *a2, size_t numBytes);
size_t OS_FileRead_hook(FILE *file, void *a2, size_t numBytes)
{

    uintptr_t calledFrom = 0;
    __asm__ volatile("mov %0, lr"
    : "=r"(calledFrom));
    calledFrom -= g_libGTASA;


    if (calledFrom == 0x1E8142 + 1 && file == (void*)lastOpenedFile)
    {
		Log("encrypt");
        lastOpenedFile = 0;

        AES_ctx ctx;
		InitCTX(ctx, &g_iEncryptionKeyTXD[0]);

        size_t retv = OS_FileRead(file, a2, numBytes);
        int fileSize = numBytes;
        int iters = fileSize / PART_SIZE_TXD;
        uintptr_t pointer = (uintptr_t)a2;
        for (int i = 0; i < iters; i++)
        {
            AES_CBC_decrypt_buffer(&ctx, (uint8_t *)pointer, PART_SIZE_TXD);
            pointer += PART_SIZE_TXD;
        }
        return retv;
    }
    if (calledFrom == 0x001E91AE + 1)
    {
        int retn = OS_FileRead(file, a2, numBytes);

        dwRLEDecompressSourceSize = *(uint32_t*)a2;

        return retn;
    }
    else
    {
        int retn = OS_FileRead(file, a2, numBytes);

        return retn;
    }
}

void (*CTimer__StartUserPause)();
void CTimer__StartUserPause_hook()
{
	Log("CTimer__StartUserPause_hook");
	// process pause event
	if (g_pJavaWrapper)
	{
		if (CKeyBoard::IsNewKeyboard())
		{
			CKeyBoard::Close();
		}
		g_pJavaWrapper->SetPauseState(true);

		CSpeedometr::tempToggle(false);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 1;
}

void (*CTimer__EndUserPause)();
void CTimer__EndUserPause_hook()
{
	// process resume event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->SetPauseState(false);
		CSpeedometr::tempToggle(true);
	}

	*(uint8_t*)(g_libGTASA + 0x0096B514) = 0;
}

extern char g_iLastBlock[512];
unsigned int(*LoadFullTexture)(uintptr_t *thiz, unsigned int entryIndex);
unsigned int LoadFullTexture_hook(uintptr_t *thiz, unsigned int entryIndex)
{
	strcpy(g_iLastBlock, *(const char **)(*((DWORD *)thiz + 7) + 0x17 * entryIndex));
    //*((DWORD *)thiz + 0x1A) = 0;
    //Log("LoadFullTexture_hook %s", *(const char **)(*((DWORD *)thiz + 7) + 0x17 * a2));
//	Log("%d", *((DWORD *)thiz + 0x1A));
//	Log("%d", *((DWORD *)thiz + 0x1B));
//    if(strcmp(*(const char **)(*((DWORD *)thiz + 7) + 0x17 * a2), "hud_bg") == 0){
//
//    }
    return LoadFullTexture(thiz, entryIndex);
}
//
uint8_t* (*RLEDecompress)(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape);
uint8_t* RLEDecompress_hook(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape) {
    if (!pDest)
    {
        return 0;
    }

    if (!pSrc)
    {
        return pDest;
    }

    uint8_t* pTempDest = pDest;
    const uint8_t* pTempSrc = pSrc;
    uint8_t* pEndOfDest = &pDest[uiDestSize];

    uint8_t* pEndOfSrc = (uint8_t*)&pSrc[dwRLEDecompressSourceSize];

    if (pDest < pEndOfDest)
    {
        do
        {
            if (*pTempSrc == uiEscape)
            {
                uint8_t ucCurSeg = pTempSrc[1];
                if (ucCurSeg)
                {
                    uint8_t* ucCurDest = pTempDest;
                    uint8_t ucCount = 0;
                    do
                    {
                        ++ucCount;
                        //Log("WRITE111 TO 0x%x FROM 0x%x SIZE %d", ucCurDest, pTempSrc + 2, uiSegSize);
                        pDest = (uint8_t*)memcpy(ucCurDest, pTempSrc + 2, uiSegSize);

                        ucCurDest += uiSegSize;
                    } while (ucCurSeg != ucCount);


                    pTempDest += uiSegSize * ucCurSeg;
                }
                pTempSrc += 2 + uiSegSize;
            }

            else
            {
                if (pTempSrc + uiSegSize >= pEndOfSrc)
                {
                    //Log("AVOID CRASH TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, pEndOfSrc - pTempSrc - 1, pEndOfSrc);
                    return pDest;
                }
                else
                {
                    //Log("WRITE222 TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, uiSegSize, pEndOfSrc);
                    pDest = (uint8_t*)memcpy(pTempDest, pTempSrc, uiSegSize);
                    pTempDest += uiSegSize;
                    pTempSrc += uiSegSize;
                }
            }
        } while (pEndOfDest > pTempDest);
    }

    dwRLEDecompressSourceSize = 0;

    return pDest;
}

void (*CPools_Initialise)(void);
void CPools_Initialise_hook(void)
{
	struct PoolAllocator {

		struct Pool {
			void* objects;
			uint8_t* flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool* p = new Pool;

			p->objects = new char[size * count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};

	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC38) = ms_pPtrNodeSingleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC3C) = ms_pPtrNodeDoubleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC40) = ms_pEntryInfoNodePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC44) = ms_pPedPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC48) = ms_pVehiclePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC4C) = ms_pBuildingPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC50) = ms_pObjectPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC54) = ms_pDummyPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC58) = ms_pColModelPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC5C) = ms_pTaskPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC60) = ms_pEventPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC64) = ms_pPointRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC68) = ms_pPatrolRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC6C) = ms_pNodeRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC70) = ms_pTaskAllocatorPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC74) = ms_pPedIntelligencePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x0095AC78) = ms_pPedAttractorPool;
}

void (*CPlaceable_InitMatrixArray)();
void CPlaceable_InitMatrixArray_hook()
{
	CHook::CallFunction<void>(g_libGTASA + 0x00407FD4 + 1, g_libGTASA + 0x0095A988, 10000);
}


int (*CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry)
		return 0;
	uint16_t size = *(uint16_t *)(resEntry + 26);
	if (size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
		if (!arr)
			return 0;
		uint32_t validFlag = flags & 0x84;
		for (int i = 0; i < size; i++)
		{
			if (!arr[i].validate)
				break;
			if (validFlag)
			{
				uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
				if (!v4)
					;
				else
				{
					if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
						return 0;
					else
					{
						if (!*(uintptr_t **)v4)
							return 0;
					}
				}
			}
		}
	}

	return CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry)
		return 0;
	uint16_t size = *(uint16_t *)(resEntry + 26);
	if (size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
		if (!arr)
			return 0;
		uint32_t validFlag = flags & 0x84;
		for (int i = 0; i < size; i++)
		{
			if (!arr[i].validate)
				break;
			if (validFlag)
			{
				uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
				if (!v4)
					;
				else
				{
					if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
						return 0;
					else
					{
						if (!*(uintptr_t **)v4)
							return 0;
					}
				}
			}
		}
	}
	return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

uintptr_t (*GetMeshPriority)(uintptr_t);
uintptr_t GetMeshPriority_hook(uintptr_t rpMesh)
{
	if (rpMesh)
	{
		RpMaterial *rpMeshMat = *(RpMaterial **)(rpMesh + 8);
		if (rpMeshMat)
		{
			if (rpMeshMat->texture)
			{
				if (!rpMeshMat->texture->raster)
					return 0;
			}
		}
	}
	return GetMeshPriority(rpMesh);
}

int (*CTextureDatabaseRuntime__GetEntry)(unsigned int *thiz, const char *name, bool *hasSibling);
int CTextureDatabaseRuntime__GetEntry_hook(unsigned int *thiz, const char *name, bool *hasSibling)
{
	//Log("GetEntry = %s", (char*)a1);
	int result; // r0

	if (thiz)
		result = CTextureDatabaseRuntime__GetEntry(thiz, name, hasSibling);
	else
		result = -1;

	return result;
}

void (*CBaseModelInfo__SetColModel)(uintptr_t* thiz, uintptr_t* pColModel, bool bOwnColModel);
void CBaseModelInfo__SetColModel_hook(uintptr_t* thiz, uintptr_t* pColModel, bool bOwnColModel)
{
	if(!thiz || !pColModel)
		return;

	CBaseModelInfo__SetColModel(thiz, pColModel, bOwnColModel);
}


void InstallSpecialHooks()
{
	Log("InstallSpecialHooks");

//	// texture Краш если с текстурами какая-то хуйня
	CHook::InlineHook(g_libGTASA, 0x001E8FF8, &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);

//	//IMG
	//CHook::InlineHook(g_libGTASA, 0x3850E4, &CBaseModelInfo__DeleteCollisionModel_hook, &CBaseModelInfo__DeleteCollisionModel);
	//CHook::InlineHook(g_libGTASA, 0x3850E4, &CBaseModelInfo__SetColModel_hook, &CBaseModelInfo__SetColModel);
	 CHook::InlineHook(g_libGTASA, 0x002CF680, &CStreaming_InitImageList_hook, (uintptr_t*)&CStreaming_InitImageList);
//
	CHook::InlineHook(g_libGTASA, 0x0021829C, &GetMeshPriority_hook, &GetMeshPriority);
//	//new fix
//	CHook::InlineHook(g_libGTASA, 0x1EEC90, &rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);
//	CHook::InlineHook(g_libGTASA, 0x28AAAC, &CustomPipeRenderCB_hook, &CustomPipeRenderCB);
//
//	//
//	// CPlaceable::InitMatrixArray
//	CHook::WriteMemory(g_libGTASA + 0x3ABB0A, "\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000
	CHook::InlineHook(g_libGTASA, 0x00408B20, &CPlaceable_InitMatrixArray_hook, &CPlaceable_InitMatrixArray);
//
	// RLEDecompress fix
	//CHook::InlineHook(g_libGTASA, 0x001E7316, &RLEDecompress_hook, &RLEDecompress);
//
//	//
	CHook::InlineHook(g_libGTASA, 0x001E9148, &LoadFullTexture_hook, &LoadFullTexture);
//
//	CHook::NOP(g_libGTASA + 0x00185DE8, 2);//_rwOpenGLNativeTextureReadXBOXPvS_
//	//
//	CHook::NOP(g_libGTASA + 0x002F9E5C, 10); 	//LoadCutsceneData
	//CHook::NOP(g_libGTASA + 0x00471EE2, 2);		//CCutsceneMgr::Initialise
//
	CHook::InlineHook(g_libGTASA, 0x00421118, &CTimer__StartUserPause_hook, &CTimer__StartUserPause);
	CHook::InlineHook(g_libGTASA, 0x00421128, &CTimer__EndUserPause_hook, &CTimer__EndUserPause);
//
//	CHook::NOP(g_libGTASA + 0x00398972, 2); // get out fucking roadblocks
//
//	//crash
//	if ( !*(uintptr_t *)(g_libGTASA + 0x61B298) && !((int (*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM") )
//	{
//		CHook::NOP(g_libGTASA + 0x1A6164, 4);
//		CHook::WriteMemory(g_libGTASA + 0x1A6164, "\x70\x47", 2);
//	}
//
//	//pvr
	CHook::UnFuck(g_libGTASA + 0x001E87A0);
	*(char*)(g_libGTASA + 0x1E87A0 + 12) = 'd';
	*(char*)(g_libGTASA + 0x1E87A0 + 13) = 'x';
	*(char*)(g_libGTASA + 0x1E87A0 + 14) = 't';
//
	CHook::UnFuck(g_libGTASA + 0x001E8C04);
	*(char*)(g_libGTASA + 0x001E8C04 + 12) = 'd';
	*(char*)(g_libGTASA + 0x001E8C04 + 13) = 'x';
	*(char*)(g_libGTASA + 0x001E8C04 + 14) = 't';
//
//	//etc
	CHook::UnFuck(g_libGTASA + 0x001E878C);
	*(char*)(g_libGTASA + 0x001E878C + 12) = 'd';
	*(char*)(g_libGTASA + 0x001E878C + 13) = 'x';
	*(char*)(g_libGTASA + 0x001E878C + 14) = 't';
//
	CHook::UnFuck(g_libGTASA + 0x001E8BF4);
	*(char*)(g_libGTASA + 0x001E8BF4 + 12) = 'd';
	*(char*)(g_libGTASA + 0x001E8BF4 + 13) = 'x';
	*(char*)(g_libGTASA + 0x001E8BF4 + 14) = 't';
//
//	CHook::WriteMemory(g_libGTASA + 0x0023BEDC, "\xF8\xB5", 2);
//	CHook::WriteMemory(g_libGTASA + 0x0023BEDE, "\x00\x46\x00\x46", 4);
//
	CHook::InlineHook(g_libGTASA, 0x002671E2, &OS_FileRead_hook, &OS_FileRead);
	//CHook::NOP(g_libGTASA + 0x3F3694, 2);
	CHook::InlineHook(g_libGTASA, 0x00266DB4, &NvFOpen_hook, & NvFOpen);
//
	CHook::InlineHook(g_libGTASA, 0x0046F570, &InitialiseRenderWare_hook, &InitialiseRenderWare);
	CHook::InlineHook(g_libGTASA, 0x0029C06C, &MainMenuScreen__Update_hook, &MainMenuScreen__Update);
	CHook::InlineHook(g_libGTASA, 0x00266A68, &OS_FileOpen_hook, &OS_FileOpen);
//
	CHook::InlineHook(g_libGTASA, 0x00570C70, &cHandlingDataMgr__FindExactWord_hook, &cHandlingDataMgr__FindExactWord);
	CHook::InlineHook(g_libGTASA, 0x00570DC8, cHandlingDataMgr__ConvertDataToGameUnits_hook, &cHandlingDataMgr__ConvertDataToGameUnits);
	//CHook::InlineHook(g_libGTASA, 0x00269634, NVEventGetNextEvent_hook, &NVEventGetNextEvent_hooked);
	CHook::InlineHook(g_libGTASA, 0x0046BB04, CStreaming__Init2_hook, &CStreaming__Init2);	// increase stream memory value
	CHook::InlineHook(g_libGTASA, 0x0040C900, &CPools_Initialise_hook, &CPools_Initialise);
}

/* =========================================== Ped damage handler =========================================== */
enum ePedPieceTypes
{
	PED_PIECE_UNKNOWN = 0,

	PED_PIECE_TORSO = 3,
	PED_PIECE_ASS,
	PED_PIECE_LEFT_ARM,
	PED_PIECE_RIGHT_ARM,
	PED_PIECE_LEFT_LEG,
	PED_PIECE_RIGHT_LEG,
	PED_PIECE_HEAD
};

struct CPedDamageResponseInterface
{
	float fDamageHealth;
	float fDamageArmor;
	bool bUnk;
	bool bForceDeath;
	bool bDamageCalculated;
	bool bUnk3;
};

struct CPedDamageResponseCalculatorInterface
{
	CEntityGta *pEntity;
	float fDamage;
	ePedPieceTypes bodyPart;
	unsigned int weaponType;
	bool bSpeak; // refers to a CPed::Say call (the dying scream?)
};

// thanks Codeesar
struct stPedDamageResponse
{
	CEntityGta* pEntity;
	float fDamage;
	int iBodyPart;
	int iWeaponType;
	bool bSpeak;
};

extern float m_fWeaponDamages[43 + 1];

void onDamage(CPedGta* issuer, CPedGta* damaged)
{
	if (!pNetGame) return;
	CPedGta* pPedPlayer = GamePool_FindPlayerPed();
	if (damaged && (pPedPlayer == issuer))
	{
		if (pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((CPedGta*)damaged) != INVALID_PLAYER_ID)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			CAMERA_AIM* caAim = pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

			CVector aim;
			aim.x = caAim->f1x;
			aim.y = caAim->f1y;
			aim.z = caAim->f1z;

			pPlayerPool->GetLocalPlayer()->SendBulletSyncData(pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)damaged), BULLET_HIT_TYPE_PLAYER, aim);
		}
	}
}

void (*CPedDamageResponseCalculator__ComputeDamageResponse)(stPedDamageResponse* thiz, CEntityGta* pEntity, uintptr_t pDamageResponse, bool bSpeak);
void CPedDamageResponseCalculator__ComputeDamageResponse_hook(stPedDamageResponse* thiz, CEntityGta* pEntity, uintptr_t pDamageResponse, bool bSpeak)
{
	if (thiz && pEntity) onDamage((CPedGta*)*(uintptr_t*)thiz, (CPedGta*)pEntity);
	int weaponid = thiz->iWeaponType;
	float fDamage;
	if( weaponid < 0 || weaponid > size(m_fWeaponDamages) )
	{
		fDamage = thiz->fDamage;
	}
	else {
		fDamage = m_fWeaponDamages[weaponid];
	}
//	float fDamage = m_fWeaponDamages[weaponid];

	int bodypart = thiz->iBodyPart;

	if(pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if(pPlayerPool)
		{
			if(weaponid < 0 || weaponid > 255 || (weaponid > 54 && weaponid < 200) || (weaponid > 201 && weaponid < 255))
				weaponid = 255; // suicide
			else if(weaponid == 18)
				weaponid = 37; // flamethower
			else if(weaponid == 35 || weaponid == 16)
				weaponid = 51; // explosion

			PLAYERID damagedid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)thiz->pEntity); // отправитель урона
			PLAYERID issuerid = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGta*)pEntity); // получатель

			PLAYERID byteLocalId = pPlayerPool->GetLocalPlayerID();

			// give player damage
			if((CPedGta*)thiz->pEntity == pGame->FindPlayerPed()->m_pPed && issuerid != INVALID_PLAYER_ID ) {
				CHUD::addGiveDamageNotify(issuerid, weaponid, fDamage);
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(false, issuerid, fDamage, weaponid,
															  bodypart);
			}

			// player take damage
			else if(damagedid != INVALID_PLAYER_ID && issuerid == INVALID_PLAYER_ID) {
				pPlayerPool->GetLocalPlayer()->GiveTakeDamage(true, damagedid, fDamage, weaponid, bodypart);

				char nick[MAX_PLAYER_NAME];
				if( pPlayerPool->GetSlotState(damagedid) )
				{
					strcpy(nick, pPlayerPool->GetPlayerName(damagedid));
				}
				else {
					strcpy(nick, "None");
				}
				CHUD::addTakeDamageNotify(pPlayerPool->GetPlayerName(damagedid), weaponid, fDamage);
			}
		}
	}

	CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pEntity, pDamageResponse, bSpeak);
}


int(*RwFrameAddChild)(int, int);
int RwFrameAddChild_hook(int a1, int a2)
{
	if (a2 && a1)
	{
		return RwFrameAddChild(a1, a2);
	}
	else return 0;
}

void (*CAnimManager__UncompressAnimation)(int, int);
void CAnimManager__UncompressAnimation_hook(int a1, int a2)
{
	if (a1)
	{
		CAnimManager__UncompressAnimation(a1, a2);
	}
}

int(*CUpsideDownCarCheck__IsCarUpsideDown)(int, int);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int a1, int a2)
{
	if (*(uintptr_t*)(a2 + 20))
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(a1, a2);
	}
	return 0;
}

int(*CAnimBlendNode__FindKeyFrame)(int, float, int, int);
int CAnimBlendNode__FindKeyFrame_hook(int a1, float a2, int a3, int a4)
{
	if (*(uintptr_t*)(a1 + 16))
	{
		return CAnimBlendNode__FindKeyFrame(a1, a2, a3, a4);
	}
	else return 0;
}


/* ====================================================== */
#include "CAtomicModelInfo.h"

/* ====================================================== */

#include "CVehicleModelInfo.h"

CStore<CVehicleModelInfo, 250> ms_vehicleModelInfoStore;

CVehicleModelInfo* (*CModelInfo_AddVehicleModel)(int index);
CVehicleModelInfo* CModelInfo_AddVehicleModel_hook(int index)
{
	Log("CModelInfo_AddVehicleModel_hook");

//	auto& pInfo = ms_vehicleModelInfoStore.AddItem();
//	pInfo.Init();
//	CModelInfo::SetModelInfo(index, &pInfo);
//	return &pInfo;
}

#include "..//keyboard.h"

uint64_t(*CWorld_ProcessPedsAfterPreRender)();
uint64_t CWorld_ProcessPedsAfterPreRender_hook()
{
	uint64_t res = CWorld_ProcessPedsAfterPreRender();
	if (pNetGame && pNetGame->GetPlayerPool())
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CPlayerPed* pPed = nullptr;
			if (pNetGame->GetPlayerPool()->GetLocalPlayerID() == i)
			{
				pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
			}
			else
			{
				if (pNetGame->GetPlayerPool()->GetSlotState(i))
				{
					pPed = pNetGame->GetPlayerPool()->GetAt(i)->GetPlayerPed();
				}
			}
			if (!pPed) continue;
			else
			{
				pPed->ProcessAttach();
			}
		}
	}

	return res;
}

int RemoveModelIDs[MAX_REMOVE_MODELS];
CVector RemovePos[MAX_REMOVE_MODELS];
float RemoveRad[MAX_REMOVE_MODELS];
int iTotalRemovedObjects = 0;


#include "util.h"

int(*CFileLoader__LoadObjectInstance)(uintptr_t, uintptr_t);
int CFileLoader__LoadObjectInstance_hook(uintptr_t thiz, uintptr_t name)
{
	for (int i = 0; i < iTotalRemovedObjects; i++)
	{
		if (RemoveModelIDs[i] == *(uint32_t*)(thiz + 28))
		{
			CVector pos;
			pos.x = *(float*)(thiz);
			pos.y = *(float*)(thiz + 4);
			pos.z = *(float*)(thiz + 8);
			if (GetDistanceBetween3DPoints(&pos, &RemovePos[i]) <= RemoveRad[i])
			{
				*(int*)(thiz + 28) = 19300;
			}
		}
	}
	return CFileLoader__LoadObjectInstance(thiz, name);
}

#include <list>

std::list<std::pair<unsigned int*, unsigned int>> resetEntries;

static uint32_t Color32Reverse(uint32_t x)
{
	return
		((x & 0xFF000000) >> 24) |
		((x & 0x00FF0000) >> 8) |
		((x & 0x0000FF00) << 8) |
		((x & 0x000000FF) << 24);
}

static RwRGBA DWORD2RGBAinternal(uint32_t dwColor)
{
	RwRGBA tmp;

	tmp.blue = dwColor & 0xFF; dwColor >>= 8;
	tmp.green = dwColor & 0xFF; dwColor >>= 8;
	tmp.red = dwColor & 0xFF; dwColor >>= 8;
	tmp.alpha = dwColor & 0xFF; /* dwColor >>= 8; */

	return tmp;
}

uintptr_t RwFrameForAllObjectsCALLBACK1(uintptr_t object, CObject* pObject)
{
	if (*(uint8_t*)object != 1)
	{
		return object;
	}
	uintptr_t pAtomic = object;
	RpGeometry* pGeom = *(RpGeometry * *)(pAtomic + 24);
	if (!pGeom)
	{
		return object;
	}

	int numMats = pGeom->matList.numMaterials;
	if (numMats > 16)
	{
		numMats = 16;
	}
	for (int i = 0; i < numMats; i++)
	{
		RpMaterial* pMat = pGeom->matList.materials[i];
		if (!pMat)
		{
			continue;
		}
		if (pObject->m_pMaterials[i].m_bCreated)
		{
			RpMaterial* pMat = pGeom->matList.materials[i];
			if (!pMat)
			{
				continue;
			}
			if (pObject->m_pMaterials[i].pTex)
			{
				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pMat->texture), *reinterpret_cast<unsigned int*>(&pMat->texture)));
				pMat->texture = pObject->m_pMaterials[i].pTex;
			}
			if (pObject->m_pMaterials[i].dwColor)
			{
				//((int(*)())(g_libGTASA + 0x00559FC8 + 1))();
				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pGeom->flags), *reinterpret_cast<unsigned int*>(&pGeom->flags)));
				pGeom->flags |= 0x00000040;
				pGeom->flags &= 0xFFFFFFF7;
				RwRGBA r = DWORD2RGBAinternal(pObject->m_pMaterials[i].dwColor);

				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pMat->color), *reinterpret_cast<unsigned int*>(&pMat->color)));
				pMat->color = r;
				pMat->surfaceProps.ambient = 1.0f;
				pMat->surfaceProps.specular = 0.0f;
				pMat->surfaceProps.diffuse = 1.0f;
			}
		}
	}
	return object;
}

int g_iLastRenderedObject;

RwTexture* (*GetTexture_orig)(const char* name);
RwTexture* GetTexture_hook(const char* name)
{
	RwTexture* result = TextureDatabaseRuntime::GetTexture(name);

	if (!result)
	{
		Log("Texture %s was not found", name);
		return nullptr;
	}
	else
	{
		Log("Texture %s", name);
		++*(int32_t *)(result + 0x54); // ++result->refCount;
		return result;
	}
}

int(*CPlayerInfo__Process)(uintptr_t *thiz, uintptr_t *a2);
int CPlayerInfo__Process_hook(uintptr_t *thiz, uintptr_t *a2)
{
	float *v23; // r3
	v23 = *(float **)thiz;
	if ( !*((BYTE *)thiz + 0xD5)  || (*((BYTE *)v23 + 0x481) & 1) == 0){
		return 0;
	}
	return CPlayerInfo__Process(thiz, a2);
}

RwFrame* CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, RpClump* pClump, int id)
{
	if (pFrameResult)
		return pFrameResult;

	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;

	if (calledFrom == 0x00515708                // CVehicle::SetWindowOpenFlag
		|| calledFrom == 0x00515730             // CVehicle::ClearWindowOpenFlag
		|| calledFrom == 0x00338698             // CVehicleModelInfo::GetOriginalCompPosition
		|| calledFrom == 0x00338B2C)            // CVehicleModelInfo::CreateInstance
		return nullptr;

	for (uint i = 2; i < 40; i++)
	{
		RwFrame* pNewFrameResult = nullptr;
		uint     uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);

		pNewFrameResult = ((RwFrame * (*)(RpClump * pClump, int id))(g_libGTASA + 0x00335CC0 + 1))(pClump, i);

		if (pNewFrameResult)
		{
			return pNewFrameResult;
		}
	}

	return nullptr;
}
RwFrame* (*CClumpModelInfo_GetFrameFromId)(RpClump*, int);
RwFrame* CClumpModelInfo_GetFrameFromId_hook(RpClump* a1, int a2)
{
	return CClumpModelInfo_GetFrameFromId_Post(CClumpModelInfo_GetFrameFromId(a1, a2), a1, a2);
}

int (*CEventHandler__HandleEvents)(uintptr_t *thiz);
int CEventHandler__HandleEvents_hook(uintptr_t *thiz)
{
	if(*((DWORD *)thiz + 1)){
		return CEventHandler__HandleEvents(thiz);
	}
	return 0;
}

void (*CWidgetRegionLook__Update)(uintptr_t thiz);
void CWidgetRegionLook__Update_hook(uintptr_t thiz)
{
	if (bBlockCWidgetRegionLookUpdate)
	{
		return;
	}
	else
	{
		CWidgetRegionLook__Update(thiz);
	}
}

void (*GivePedScriptedTask)(uintptr_t* thiz, int pedHandle, uintptr_t* a3, int commandID);
void GivePedScriptedTask_hook(uintptr_t* thiz, int pedHandle, uintptr_t* a3, int commandID)
{
	//Log("pedHandle = %d, local = %d", pedHandle, pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_dwGTAId);
	if(!pedHandle || !commandID || !a3)return;

	if ( pedHandle == 0xFFFFFFFF ){
		return GivePedScriptedTask(thiz, pedHandle, a3, commandID);
	}
	//if(!(pedHandle>>8))return;
	uintptr_t tmp = *(uintptr_t*)(g_libGTASA + 0x008B93D4);

	if(  !(*(const CPedGta**) tmp + 0x4 ) ) return; // CPools::ms_pPedPool

	GivePedScriptedTask(thiz, pedHandle, a3, commandID);
}

int (*CObject__ProcessGarageDoorBehaviour)(uintptr_t, int);
int CObject__ProcessGarageDoorBehaviour_hook(uintptr_t thiz, int a2)
{
	if (thiz)
		if (!*(uintptr_t *)(thiz + 372))
			return 0;
	return CObject__ProcessGarageDoorBehaviour(thiz, a2);
}

int (*CTaskSimpleCarOpenDoorFromOutside__ComputeAnimID)(uintptr_t *thiz, int *a2, int *a3, int a4);
int CTaskSimpleCarOpenDoorFromOutside__ComputeAnimID_hook(uintptr_t *thiz, int *a2, int *a3, int a4)
{
	if( !(*((DWORD *)thiz + 4)) )return 0;

	return CTaskSimpleCarOpenDoorFromOutside__ComputeAnimID(thiz, a2, a3, a4);
}

int (*RpMaterialDestroy)(int a1, int a2, int a3, int a4);
int RpMaterialDestroy_hook(int a1, int a2, int a3, int a4)
{
	if(!a1 || !a2 || !a3 || !a4)return 1;

	return RpMaterialDestroy(a1, a2, a3, a4);
}

#include "..//crashlytics.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;

char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, char* a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, char* a2)
{
	if (thiz && a2)
	{
		uintptr_t* dwRenderQueue = (uintptr_t*)thiz;

		memset(g_bufRenderQueueCommand, 0, sizeof(g_bufRenderQueueCommand));

		g_dwRenderQueueOffset = *(uintptr_t*)a2;
		snprintf(g_bufRenderQueueCommand, 190, "offset: %d | name: %s", g_dwRenderQueueOffset, (const char*)(*(dwRenderQueue + 100 + g_dwRenderQueueOffset)));

		return RenderQueue__ProcessCommand(thiz, a2);
	}
	else
	{
		return nullptr;
	}
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_hook(int a1, unsigned int a2)
{
	if (a1)
	{
		return _rwFreeListFreeReal(a1, a2);
	}
	else
	{
		return 0;
	}
}

uintptr_t* CTouchInterface__m_bWidgets;


std::list<std::pair<unsigned int*, unsigned int>> resetEntriesVehicle;


RpMaterial* CVehicle__SetupRenderMatCB(RpMaterial* material, void* data)
{
	if (material)
	{
		if (material->texture)
		{
			CVehicle* pVeh = (CVehicle*)data;

			for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
			{
				if (pVeh->m_bReplaceTextureStatus[i])
				{
					if (!strcmp(&(material->texture->name[0]), &(pVeh->m_szReplacedTextures[i].szOld[0])))
					{
						if (pVeh->m_szReplacedTextures[i].pTexture)
						{
							resetEntriesVehicle.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&(material->texture)), *reinterpret_cast<unsigned int*>(&(material->texture))));
							material->texture = pVeh->m_szReplacedTextures[i].pTexture;
							if (strstr(pVeh->m_szReplacedTextures[i].szOld, "ret_t"))
							{
								material->color.alpha = 255;
							}
						}
					}
				}
			}
		}
	}
	return material;
}

uintptr_t CVehicle__SetupRenderCB(uintptr_t atomic, void* data)
{
	if (*(RpGeometry * *)(atomic + 24))
	{
		((RpGeometry * (*)(RpGeometry*, uintptr_t, void*))(g_libGTASA + 0x001E284C + 1))(*(RpGeometry * *)(atomic + 24), (uintptr_t)CVehicle__SetupRenderMatCB, (void*)data); // RpGeometryForAllMaterials
	}

	return atomic;
}
#include "..//cryptors/MODELINFO_EDITABLE_result.h"

void (*CVehicleModelInfo__SetEditableMaterials)(uintptr_t);
void CVehicleModelInfo__SetEditableMaterials_hook(uintptr_t clump)
{
	PROTECT_CODE_MODELINFO_EDITABLE;
	RpClump *pClump = (RpClump *)clump;

	if (pNetGame && pClump)
	{
		if (pNetGame->GetVehiclePool())
		{
			VEHICLEID vehId = pNetGame->GetVehiclePool()->FindIDFromRwObject((RwObject *)clump);
			CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVehicle)
			{
				if (pVehicle->m_bReplacedTexture)
				{
					// RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* pData);
					((RpClump * (*)(RpClump *, uintptr_t, void *))(g_libGTASA + 0x1E0EA0 + 1))(pClump, (uintptr_t)CVehicle__SetupRenderCB, (void *)pVehicle); // RpClumpForAllAtomics
				}
			}
		}
	}

	CVehicleModelInfo__SetEditableMaterials(clump);
}

#include "..//cryptors/RESET_AFTER_RENDER_result.h"

void (*CVehicle__ResetAfterRender)(uintptr_t);
void CVehicle__ResetAfterRender_hook(uintptr_t thiz)
{
	PROTECT_CODE_RESET_AFTER_RENDER;

	for (auto& p : resetEntriesVehicle)
	{
		*p.first = p.second;
	}
	resetEntriesVehicle.clear();

	CVehicle__ResetAfterRender(thiz);
}

#include "CRenderTarget.h"
#include "..//gui/CFontInstance.h"
#include "..//gui/CFontRenderer.h"
#include "CCustomPlateManager.h"

static bool g_bFirstPersonOnFootEnabled = false;
extern bool bDisableTestGovno;
void (*CGame__Process)();
void CGame__Process_hook()
{
	CGame__Process();

	if(pGame->bIsGameExiting)return;

	static bool once = false;
	if (!once)
	{
		CCustomPlateManager::Initialise();
		//CSnow::Initialise();
		once = true;
	}

	CCustomPlateManager::Process();
}

#include "..//cryptors/AUTOMOBILE_COLLISION_result.h"

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
void (*CAutomobile__ProcessEntityCollision)(CVehicleGta* a1, CEntityGta* a2, int a3);
void CAutomobile__ProcessEntityCollision_hook(CVehicleGta* a1, CEntityGta* a2, int a3)
{
	PROTECT_CODE_AUTOMOBILE_COLLISION;

	g_usLastProcessedModelIndexAutomobile = a1->nModelIndex;
	g_iLastProcessedModelIndexAutoEnt = a2->nModelIndex;

	bool bReplace = false;
	void* pOld = nullptr;
	uint8_t* pColData = nullptr;

	

	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehId = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(a1);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{
				if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
				{
					pColData = GetCollisionDataFromModel(a1->nModelIndex);
					if (pColData && pVeh->m_pSuspensionLines)
					{
						if (*(void**)(pColData + 16))
						{
							pOld = *(void**)(pColData + 16);
							*(void**)(pColData + 16) = pVeh->m_pSuspensionLines;
							bReplace = true;
						}
					}
				}
			}
		}
	}
	CAutomobile__ProcessEntityCollision(a1, a2, a3);
	if (bReplace)
	{
		*(void**)(pColData + 16) = pOld;
	}
}

//void MainMenuScreen__OnExit(uintptr_t* thiz)
//{
//	pGame->bIsGameExiting = true;
//
//	//delete pNetGame;
//
//	//g_pJavaWrapper->ExitGame();
//}
void (*MainMenuScreen__OnExit)();
void MainMenuScreen__OnExit_hook()
{
	pGame->exitGame();

	//g_pJavaWrapper->ExitGame();

	//return CGame__Shutdown();
}

// TODO: VEHICLE RESET SUSPENSION
void (*CShadows__StoreCarLightShadow)(CVehicleGta* vehicle, int id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle);
void CShadows__StoreCarLightShadow_hook(CVehicleGta* vehicle, int id, RwTexture* texture, CVector* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle)
{
	uint8_t r, g, b;
	r = red;
	g = green;
	b = blue;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(vehicle);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	CShadows__StoreCarLightShadow(vehicle, id, texture, posn, frontX, frontY, sideX, sideY, r, g, b, maxViewAngle);
}

void CVehicle__DoHeadLightReflectionTwin(CVehicle* pVeh, MATRIX4X4* a2)
{
	CVehicleGta* v2; // r4
	int v3; // r2
	MATRIX4X4* v4; // r5
	float* v5; // r3
	float v6; // s12
	float v7; // s5
	float* v8; // r2
	float v9; // r0
	float v10; // r1
	float v11; // r2
	float v12; // s14
	float v13; // s11
	float v14; // s15
	float v15; // s13
	float v16; // s10
	float v17; // s12
	float v18; // s15
	float v19; // ST08_4


	v2 = pVeh->m_pVehicle;
	v3 = *((uintptr_t*)v2 + 5);
	v4 = a2;
	v5 = *(float**)(CModelInfo::ms_modelInfoPtrs[v2->nModelIndex] + 116);
	v6 = *v5;
	v7 = v5[1];
	if (v3)
		v8 = (float*)(v3 + 48);
	else
		v8 = (float*)((char*)v2 + 4);
	v9 = *v8;
	v10 = v8[1];
	v11 = v8[2];
	v12 = *((float*)v4 + 5);
	v13 = *((float*)v4 + 4);
	v14 = (float)(v12 * v12) + (float)(v13 * v13);
	if (v14 != 0.0)
		v14 = 1.0 / sqrtf(v14);
	v15 = v6 * 4.0;
	v16 = (float)(v15 + v15) + 1.0;
	v17 = v13 * v14;
	v18 = v12 * v14;

	v19 = v15 * v18;

	CVector pos;
	memcpy(&pos, &(v2->mat->pos), sizeof(CVector));
	pos.z += 2.0f;

	CShadows__StoreCarLightShadow(
		v2,
		(uintptr_t)v2 + 24,
		pVeh->m_Shadow.pTexture,
		&pos,
		(float)(v15 + v15) * v17 * pVeh->m_Shadow.fSizeX,
		(float)(v15 + v15) * v18 * pVeh->m_Shadow.fSizeX,
		v19 * pVeh->m_Shadow.fSizeY,
		-(float)(v15 * v17) * pVeh->m_Shadow.fSizeY,
		pVeh->m_Shadow.r, pVeh->m_Shadow.g, pVeh->m_Shadow.b,
		7.0f);
}

void (*CVehicle__DoHeadLightBeam)(CVehicleGta* vehicle, int arg0, MATRIX4X4& matrix, unsigned char arg2);
void CVehicle__DoHeadLightBeam_hook(CVehicleGta* vehicle, int arg0, MATRIX4X4& matrix, unsigned char arg2)
{
	uint8_t r, g, b;
	r = 0xFF;
	g = 0xFF;
	b = 0xFF;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(vehicle);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	*(uint8_t*)(g_libGTASA + 0x9BAA70) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAA71) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAA72) = b;

	*(uint8_t*)(g_libGTASA + 0x9BAA94) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAA95) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAA96) = b;

	*(uint8_t*)(g_libGTASA + 0x9BAB00) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAB01) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAB02) = b;

	CVehicle__DoHeadLightBeam(vehicle, arg0, matrix, arg2);

}

static CVehicle* g_pLastProcessedVehicleMatrix = nullptr;
static int g_iLastProcessedWheelVehicle = -1;

void (*CMatrix__Rotate)(void* thiz, float a1, float a2, float a3);
void CMatrix__Rotate_hook(void* thiz, float a1, float a2, float a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x003A9D76 + 1)
	{
		CMatrix__Rotate(thiz, a1, a2, a3);
		return;
	}

	CMatrix__Rotate(thiz, a1, a2, a3);
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_pLastProcessedVehicleMatrix->m_bWheelAlignmentX || g_pLastProcessedVehicleMatrix->m_bWheelAlignmentY)
		{
			if (g_iLastProcessedWheelVehicle == 2)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 4)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 5)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 7)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
		}
	}
}

void (*CMatrix__SetScale)(void* thiz, float x, float y, float z);
void CMatrix__SetScale_hook(void* thiz, float x, float y, float z)
{
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_iLastProcessedWheelVehicle >= 2 || g_iLastProcessedWheelVehicle <= 7)
		{
			// front wheel
			if (g_pLastProcessedVehicleMatrix->m_bWheelSize)
			{
				y *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
				z *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
			}
			if (g_pLastProcessedVehicleMatrix->m_bWheelWidth)
			{
				x = g_pLastProcessedVehicleMatrix->m_fWheelWidth;
			}
		}
	}

	CMatrix__SetScale(thiz, x, y, z);
}

void (*CAutomobile__UpdateWheelMatrix)(CVehicleGta* thiz, int, int);
void CAutomobile__UpdateWheelMatrix_hook(CVehicleGta* thiz, int nodeIndex, int flags)
{
	if (g_pLastProcessedVehicleMatrix)
	{
		g_iLastProcessedWheelVehicle = nodeIndex;
	}

	CAutomobile__UpdateWheelMatrix(thiz, nodeIndex, flags);
}

void (*CAutomobile__PreRender)(CVehicleGta* thiz);
void CAutomobile__PreRender_hook(CVehicleGta* thiz)
{
	uint8_t* pModelInfoStart = reinterpret_cast<uint8_t *>(CModelInfo::ms_modelInfoPtrs[thiz->nModelIndex]);

	float fOldFront = *(float*)(pModelInfoStart + 88);
	float fOldRear = *(float*)(pModelInfoStart + 92);
	CVehicle* pVeh = nullptr;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(thiz);
			pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessWheelsOffset();
				g_pLastProcessedVehicleMatrix = pVeh;

				if (pVeh->m_bWheelSize)
				{
					*(float*)(pModelInfoStart + 92) = pVeh->m_fWheelSize;
					*(float*)(pModelInfoStart + 88) = pVeh->m_fWheelSize;
				}
				if (pVeh->m_bShadow && pVeh->m_Shadow.pTexture)
				{
					CVehicle__DoHeadLightReflectionTwin(pVeh, pVeh->m_pVehicle->mat);
				}
			}
		}
	}

	CAutomobile__PreRender(thiz);

	*(float*)(pModelInfoStart + 88) = fOldFront;
	*(float*)(pModelInfoStart + 92) = fOldRear;

	g_pLastProcessedVehicleMatrix = nullptr;
	g_iLastProcessedWheelVehicle = -1;
}

#include "..//cryptors/INSTALLHOOKS_result.h"

void (*CTaskSimpleUseGun__RemoveStanceAnims)(void* thiz, void* ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(void* thiz, void* ped, float a3)
{
	uint32_t v5 = *((uint32_t*)thiz + 11);
	if (v5)
	{
		if (*(uint32_t*)(v5 + 20))
		{
			CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
	}
}

float (*CRadar__LimitRadarPoint)(float* a1);
float CRadar__LimitRadarPoint_hook(float* a1)
{
	if (*(uint8_t*)(g_libGTASA + 0x0063E0B4))
	{
		return sqrtf((float)(a1[1] * a1[1]) + (float)(*a1 * *a1));
	}

	if (!CRadarRect::IsEnabled())
	{
		return CRadar__LimitRadarPoint(a1);
	}
	float value = CRadarRect::CRadar__LimitRadarPoint_hook(a1);

	return value;
}

static int g_iCurrentWanted = 0;
static float g_fInitialPos = 0.0f;

void (*CSprite2d__Draw)(CSprite2d* a1, CRect* a2, CRGBA* a3);
void CSprite2d__Draw_hook(CSprite2d* a1, CRect* a2, CRGBA* a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (!pGUI)
	{
		return CSprite2d__Draw(a1, a2, a3);
	}

	//if (dwRetAddr == 0x003D3796 + 1 || dwRetAddr == 0x003D376C + 1 || dwRetAddr == 0x003D373E + 1 || dwRetAddr == 0x003D3710 + 1)
	///{
		if(!CHUD::bIsShow)return;
		if (CRadarRect::m_pDiscTexture == nullptr)
		{
			CRadarRect::m_pDiscTexture = a1->m_pRwTexture;
		}
		if (CRadarRect::IsEnabled() && CRadarRect::m_pRectTexture)
		{
			a1->m_pRwTexture = CRadarRect::m_pRectTexture;
			//CChatWindow::AddDebugMessage("Replacing for rect");
		}
		else
		{
			a1->m_pRwTexture = CRadarRect::m_pDiscTexture;
			//CChatWindow::AddDebugMessage("Replacing for disc");
		}


		
	//}

	CSprite2d__Draw(a1, a2, a3);
}

void (*CCam__Process)(uintptr_t);
void CCam__Process_hook(uintptr_t thiz)
{

	CVector vecSpeed;
	CVehicle* pVeh = nullptr;
	float pOld = *(float*)(g_libGTASA + 0x00608558);
	if (pNetGame && (*(uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) && CFirstPersonCamera::IsEnabled())
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					pVeh->GetMoveSpeedVector(&vecSpeed);
					CVector vec;
					vec.x = vecSpeed.x * 6.0f;
					vec.y = vecSpeed.y * 6.0f;
					vec.z = vecSpeed.z * 6.0f;
					pVeh->SetMoveSpeedVector(vec);
					*(float*)(g_libGTASA + 0x00608558) = 200.0f;
				}
			}
		}
	}

	CCam__Process(thiz);
	if (pVeh)
	{
		pVeh->SetMoveSpeedVector(vecSpeed);
		*(float*)(g_libGTASA + 0x00608558) = pOld;
	}
	if (*(uint16_t*)(thiz + 14) == 4 || *(uint16_t*)(thiz + 14) == 53) // 53 is weapon
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
					}
				}
			}
		}
	}
	if(* (uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) // in car
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, pVeh);
					}
				}
			}
		}
	}
}
int g_iCounterVehicleCamera = 0;
int (*CPad__CycleCameraModeDownJustDown)(void*);
int CPad__CycleCameraModeDownJustDown_hook(void* thiz)
{
	if (!g_pWidgetManager)
	{
		return 0;
	}

	CPedGta* pPed = GamePool_FindPlayerPed();
	if (!pPed)
	{
		return 0;
	}

	static uint32_t lastTick = GetTickCount();
	bool bPressed = false;
	if (CHUD::bIsTouchCameraButt && GetTickCount() - lastTick >= 250)
	{
		CHUD::bIsTouchCameraButt = false;
		bPressed = true;
		lastTick = GetTickCount();
	}

	if (pPed->dwAction == ePedState::PEDSTATE_DRIVING || pPed->dwAction == ePedState::PEDSTATE_PASSENGER)
	{
		if (bPressed)
		{
			g_iCounterVehicleCamera++;
		}
		if (g_iCounterVehicleCamera == 6)
		{
			CFirstPersonCamera::SetEnabled(true);
			return 0;
		}
		else if (g_iCounterVehicleCamera >= 7)
		{
			g_iCounterVehicleCamera = 0;
			CFirstPersonCamera::SetEnabled(false);
			return 1;

		}
		else
		{
			CFirstPersonCamera::SetEnabled(false);
		}

		return bPressed;
	}
	return 0;
}

void (*FxEmitterBP_c__Render)(uintptr_t* a1, int a2, int a3, float a4, char a5);
void FxEmitterBP_c__Render_hook(uintptr_t* a1, int a2, int a3, float a4, char a5)
{
	if(!a1 || !a2) return;
	uintptr_t* temp = *((uintptr_t**)a1 + 3);
	if (!temp)
	{
		return;
	}
	FxEmitterBP_c__Render(a1, a2, a3, a4, a5);
}

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

void (*CPed__ProcessEntityCollision)(CPedGta* thiz, CEntityGta* ent, void* colPoint);
void CPed__ProcessEntityCollision_hook(CPedGta* thiz, CEntityGta* ent, void* colPoint)
{
	g_iLastProcessedSkinCollision = thiz->nModelIndex;
	g_iLastProcessedEntityCollision = ent->nModelIndex;

	CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

int (*CTaskSimpleGetUp__ProcessPed)(uintptr_t* thiz, CPedGta* ped);
int CTaskSimpleGetUp__ProcessPed_hook(uintptr_t* thiz, CPedGta* ped)
{
	if(!ped)return 0;

	return CTaskSimpleGetUp__ProcessPed(thiz, ped);
}

int (*emu_ArraysDelete)(unsigned int a1, int a2, int a3, int a4);
int emu_ArraysDelete_hook(unsigned int a1, int a2, int a3, int a4)
{
	if (a1 > g_libGTASA)
		return emu_ArraysDelete(a1, a2, a3, a4);

	return 0;
}

int (*_RwTextureDestroy)(RwTexture *);
int _RwTextureDestroy_hook(RwTexture *texture)
{
	if (!texture)
		return 1;
	return _RwTextureDestroy(texture);
}

int (*RwResourcesFreeResEntry)(int);
int RwResourcesFreeResEntry_hook(int a1)
{
	int result; // r0

	if (a1)
		result = RwResourcesFreeResEntry(a1);
	else
		result = 0;
	return result;
}

int (*emu_ArraysGetID)(unsigned int);
int emu_ArraysGetID_hook(unsigned int a1)
{
	bool v1;	// zf
	int result; // r0

	v1 = a1 == 0;
	if (a1)
		v1 = *(uint32_t *)(a1 + 36) == 0;
	if (v1)
		result = 0;
	else
		result = emu_ArraysGetID(a1);
	return result;
}

int (*SetCompAlphaCB)(int, int);
int SetCompAlphaCB_hook(int a1, char a2)
{
	int result; // r0

	if (a1)
		result = SetCompAlphaCB(a1, a2);
	else
		result = 0;
	return result;
}

void (*CEntity__RegisterReference)(CEntityGta *thiz, CEntityGta **a2);
void CEntity__RegisterReference_hook(CEntityGta *thiz, CEntityGta **a2)
{
	Log("CEntity__RegisterReference");
	if(!thiz)return;
	if(!a2)return;
//    if( !*((DWORD *)thiz + 4)  )return false;
//	if(! thiz + 0x36)return false;

	CEntity__RegisterReference(thiz, a2);
}

float (*CDraw__SetFOV)(float thiz, float a2);
float CDraw__SetFOV_hook(float thiz, float a2)
{
	float tmp = (float)((float)((float)(*(float *)&*(float *)(g_libGTASA + 0x0098525C) - 1.3333) * 11.0) / 0.44444) + thiz;
	if(tmp > 100) tmp = 100.0;
	*(float *)(g_libGTASA + 0x00610968) = tmp;
	return thiz;
}

int (*MobileSettings__GetMaxResWidth)();
int MobileSettings__GetMaxResWidth_hook()
{
	//Log("res = %d", ((int(*)())(g_libGTASA + 0x0023816C + 1))());
	return (int)( ((int(*)())(g_libGTASA + 0x0023816C + 1))()/1.1 );
}

char **(*CPhysical__Add)(uintptr_t thiz);
char **CPhysical__Add_hook(uintptr_t thiz)
{
	// char **result = 0;

	if (pNetGame)
	{
		CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_aAttachedObjects[i].bState)
				{
					if (pPlayerPed->m_aAttachedObjects[i].pObject)
						if ((uintptr_t)pPlayerPed->m_aAttachedObjects[i].pObject->m_pEntity == thiz)
						{
							CObject *pObject = pPlayerPed->m_aAttachedObjects[i].pObject;
							if (pObject->m_pEntity->mat->pos.x > 20000.0f ||
							pObject->m_pEntity->mat->pos.y > 20000.0f ||
							pObject->m_pEntity->mat->pos.z > 20000.0f ||
								pObject->m_pEntity->mat->pos.x < -20000.0f ||
								pObject->m_pEntity->mat->pos.y < -20000.0f ||
								pObject->m_pEntity->mat->pos.z < -20000.0f)
							{
								/*if(pChatWindow)
								{
									pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
								}*/
								return 0;
							}
							// Log("Processing local attached object");
							// result = CPhysical__Add(thiz);
						}
				}
			}
		}

		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

		if (pVehiclePool)
		{
			for (size_t i = 0; i < MAX_VEHICLES; i++)
			{
				if (pVehiclePool->GetSlotState(i))
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(i);
					if (pVehicle && pVehicle->IsAdded())
					{
						/*CObject* pObject = pVehicle->Att((CEntityGta*)thiz);
						if (pObject != nullptr)
						{
							if (pObject->m_pEntity->mat->pos.x > 20000.0f || pObject->m_pEntity->mat->pos.y > 20000.0f || pObject->m_pEntity->mat->pos.z > 20000.0f ||
								pObject->m_pEntity->mat->pos.x < -20000.0f || pObject->m_pEntity->mat->pos.y < -20000.0f || pObject->m_pEntity->mat->pos.z < -20000.0f)
							{
								/*if(pChatWindow)
								{
									pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
								}
								return 0;
							}
						}*/
					}
				}
			}
		}

		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_aAttachedObjects[i].bState)
								{
									if ((uintptr_t)pPlayerPed->m_aAttachedObjects[i].pObject->m_pEntity == thiz)
									{
										CObject *pObject = pPlayerPed->m_aAttachedObjects[i].pObject;
										if (pObject->m_pEntity->mat->pos.x > 20000.0f
											||	pObject->m_pEntity->mat->pos.y > 20000.0f
											|| pObject->m_pEntity->mat->pos.z > 20000.0f
											||	pObject->m_pEntity->mat->pos.x < -20000.0f
											|| pObject->m_pEntity->mat->pos.y < -20000.0f
											|| pObject->m_pEntity->mat->pos.z < -20000.0f)
										{
											/*if(pChatWindow)
											{
												pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
											}*/
											return 0;
										}
										// Log("Processing remote attached object. Player: %d", i);
										// Log("is added: %d | model index: %d | gta id: %d | x: %.2f | y: %.2f | z: %.2f | flags: %d | vtable: %d | unk: %d", pObject->IsAdded(), pObject->GetModelIndex(), pObject->m_dwGTAId, pObject->m_pEntity->mat->pos.x, pObject->m_pEntity->mat->pos.y, pObject->m_pEntity->mat->pos.z, pObject->m_pEntity->dwProcessingFlags, pObject->m_pEntity->vtable, pObject->m_pEntity->dwUnkModelRel);
										// result = CPhysical__Add(thiz);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/*if(result == 0)
	{
		Log("Processing unknown object.");
		result = CPhysical__Add(thiz);
	}*/

	// Log("Processed.");

	return CPhysical__Add(thiz);
}


int (*CCollision__ProcessVerticalLine)(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8);
int CCollision__ProcessVerticalLine_hook(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8)
{
	int result; // r0

	if (a3)
		result = CCollision__ProcessVerticalLine(a1, a2, a3, a4, a5, a6, a7, a8);
	else
		result = 0;
	return result;
}


int (*CWeapon__GenerateDamageEvent)(CPedGta *victim, CEntityGta *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction);
int CWeapon__GenerateDamageEvent_hook(CPedGta *victim, CEntityGta *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction)
{
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Disable ComputeDamageAnim when it's not a head!
		CHook::NOP(g_libGTASA + 0x566B30, 2);
		CHook::NOP(g_libGTASA + 0x566B88, 2);
	}
	int result = CWeapon__GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction);
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Restore bytes
		CHook::WriteMemory(g_libGTASA + 0x566B30, (uintptr_t) "\xC1\xF5\xD8\xF9", 4);
		CHook::WriteMemory(g_libGTASA + 0x566B88, (uintptr_t) "\xBB\xF5\xFE\xF8", 4);
	}
	return result;
}


int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, uintptr_t a2);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, uintptr_t a2)
{
	unsigned char v1 = *((unsigned char*)thiz + 13);
	bool bChangeTheResult = false;

	CPedGta* pPedPlayer = (CPedGta*)a2;
	if(pPedPlayer && pNetGame)
	{
		if(v1 == 0)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if(pPlayerPool)
			{
				CPlayerPed *pPlayerPed;
				if(pPedPlayer == GamePool_FindPlayerPed())
					pPlayerPed = pGame->FindPlayerPed();
				else
				{
					PLAYERID playerId = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
					if(playerId != INVALID_PLAYER_ID)
					{
						CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
						if(pRemotePlayer)
							pPlayerPed = pRemotePlayer->GetPlayerPed();
					}
				}

				if(pPlayerPed)
				{
					if((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
					{
						CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
						if(pVehiclePool)
						{
							for(VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
							{
								if(pVehiclePool->GetSlotState(veh))
								{
									CVehicle* pVehicle = pVehiclePool->GetAt(veh);
									if(pVehicle)
									{
										MATRIX4X4 vehicleMat, playerMat;
										pVehicle->GetMatrix(&vehicleMat);
										pPlayerPed->GetMatrix(&playerMat);

										float fSX = (vehicleMat.pos.x - playerMat.pos.x) * (vehicleMat.pos.x - playerMat.pos.x);
										float fSY = (vehicleMat.pos.y - playerMat.pos.y) * (vehicleMat.pos.y - playerMat.pos.y);
										float fSZ = (vehicleMat.pos.z - playerMat.pos.z) * (vehicleMat.pos.z - playerMat.pos.z);

										float fDistance = (float)sqrt(fSX + fSY + fSZ);

										if(fDistance <= 100.0f)
											*((unsigned char*)thiz + 13) |= 1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CTaskSimpleUseGun__SetPedPosition(thiz, a2);
}

int (*CTaskSimpleCarSetPedInAsDriver__ProcessPed)(uintptr_t *thiz, CPedGta *a2);
int CTaskSimpleCarSetPedInAsDriver__ProcessPed_hook(uintptr_t *thiz, CPedGta *a2)
{
	if(!a2)return false;
    if( !*((DWORD *)thiz + 4)  )return false;

	return CTaskSimpleCarSetPedInAsDriver__ProcessPed(thiz, a2);
}

uint32_t (*CVehicle__GetVehicleLightsStatus)(CVehicleGta *_pVehicle);
uint32_t CVehicle__GetVehicleLightsStatus_hook(CVehicleGta *_pVehicle)
{
//	*(bool*)(g_libGTASA + 0x97180D) = false;
//	//unsigned int v6; // r2
//	//int v7; // r3
//	//v6 = *(uint8_t *)(_pVehicle + 0x428);
//	//(v6 >> 6) = 0;
//	_pVehicle->byteFlags |= 0x40;
//	_pVehicle->m_nOverrideLights = 0;

	if(!pNetGame) return false;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    if(!pVehiclePool) return false;

    CVehicle *pVehicle = pVehiclePool->FindVehicle(_pVehicle);
	if(!pVehicle)return false;

	return pVehicle->GetLightsState();
}


void (*RenderEffects)();
void RenderEffects_hook()
{
	// fix render spheres
//	if(pSettings->Get().bnoFX)
//	{
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)1);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

		CHook::CallFunction<void>(g_libGTASA + 0x54C330 + 1);
		CHook::CallFunction<void>(g_libGTASA + 0x54D738 + 1);
//	}
//	else
//	{
//		RenderEffects();
//	}
}

uint32_t (*CWeapon__FireSniper)(CWeaponGta* pWeaponSlot, CPedGta *pFiringEntity, CEntityGta *a3, CVector *vecOrigin);
uint32_t CWeapon__FireSniper_hook(CWeaponGta* pWeaponSlot, CPedGta *pFiringEntity, CEntityGta *a3, CVector *vecOrigin)
{
	if(GamePool_FindPlayerPed() == pFiringEntity)
	{
		if(pGame)
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
				pPlayerPed->FireInstant();
		}
	}

	return 1;
}

CVector& (*FindPlayerSpeed)(int a1);
CVector& FindPlayerSpeed_hook(int a1)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x3DF20E + 1)
	{
		if(pNetGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed &&
			   pPlayerPed->IsInVehicle() &&
			   pPlayerPed->IsAPassenger())
			{
				CVector vec = {0.1f, 0.1f, 0.1f};
				return vec;
			}
		}
	}

	return FindPlayerSpeed(a1);
}

void SendBulletSync(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CEntityGta **ppEntity)
{
	BULLET_DATA bulletData;
	memset(&bulletData, 0, sizeof(BULLET_DATA));

	bulletData.vecOrigin.x = vecOrigin->x;
	bulletData.vecOrigin.y = vecOrigin->y;
	bulletData.vecOrigin.z = vecOrigin->z;

	bulletData.vecPos.x = vecPos->x;
	bulletData.vecPos.y = vecPos->y;
	bulletData.vecPos.z = vecPos->z;

	if(ppEntity)
	{
		static CEntityGta *pEntity;
		pEntity = *ppEntity;
		if(pEntity)
		{
			if(pEntity->mat)
			{
				if(pNetGame->m_iLagCompensation)
				{
					bulletData.vecOffset.x = vecPos->x - pEntity->mat->pos.x;
					bulletData.vecOffset.y = vecPos->y - pEntity->mat->pos.y;
					bulletData.vecOffset.z = vecPos->z - pEntity->mat->pos.z;
				}
				else
				{
					static MATRIX4X4 mat1;
					memset(&mat1, 0, sizeof(mat1));

					static MATRIX4X4 mat2;
					memset(&mat2, 0, sizeof(mat2));

					RwMatrixOrthoNormalize(&mat2, pEntity->mat);
					RwMatrixInvert(&mat1, &mat2);
					ProjectMatrix(&bulletData.vecOffset, &mat1, vecPos);
				}
			}

			bulletData.pEntity = pEntity;
		}
	}

	pGame->FindPlayerPed()->ProcessBulletData(&bulletData);

}

bool g_bForceWorldProcessLineOfSight = false;
uint32_t (*CWeapon__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, CWeaponGta *pWeaponSlot, CPedGta **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
uint32_t CWeapon__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, CWeaponGta *pWeaponSlot, CPedGta **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x567AFC+1 || dwRetAddr == 0x5681BA+1 || dwRetAddr == 0x5688D0+1 || dwRetAddr == 0x568DB8+1)
		g_bForceWorldProcessLineOfSight = true;

	return CWeapon__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, pWeaponSlot, ppEntity2, b1, b2, b3, b4, b5, b6, b7);
}

uint32_t (*CWorld__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8);
uint32_t CWorld__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x55E2FE + 1 || g_bForceWorldProcessLineOfSight)
	{
		g_bForceWorldProcessLineOfSight = false;

		CEntityGta *pEntity = nullptr;
		MATRIX4X4 *pMatrix = nullptr;
		CVector vecPosPlusOffset;

		if(pNetGame->m_iLagCompensation != 2)
		{
			if(g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if(g_pCurrentBulletData)
				{
					if(g_pCurrentBulletData->pEntity)
					{
						if(!CUtil::IsGameEntityArePlaceable(g_pCurrentBulletData->pEntity))
						{
							pMatrix = g_pCurrentBulletData->pEntity->mat;
							if(pMatrix)
							{
								if(pNetGame->m_iLagCompensation)
								{
									vecPosPlusOffset.x = pMatrix->pos.x + g_pCurrentBulletData->vecOffset.x;
									vecPosPlusOffset.y = pMatrix->pos.y + g_pCurrentBulletData->vecOffset.y;
									vecPosPlusOffset.z = pMatrix->pos.z + g_pCurrentBulletData->vecOffset.z;
								}
								else ProjectMatrix(&vecPosPlusOffset, pMatrix, &g_pCurrentBulletData->vecOffset);

								vecEnd->x = vecPosPlusOffset.x - vecOrigin->x + vecPosPlusOffset.x;
								vecEnd->y = vecPosPlusOffset.y - vecOrigin->y + vecPosPlusOffset.y;
								vecEnd->z = vecPosPlusOffset.z - vecOrigin->z + vecPosPlusOffset.z;
							}
						}
					}
				}
			}
		}

		uint32_t result = 0;
		result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

		if(pNetGame->m_iLagCompensation == 2)
		{
			if(g_pCurrentFiredPed)
			{
				if(g_pCurrentFiredPed == pGame->FindPlayerPed())
					SendBulletSync(vecOrigin, vecEnd, vecPos, (CEntityGta**)ppEntity);
			}

			return result;
		}

		if(g_pCurrentFiredPed)
		{
			if(g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if(g_pCurrentBulletData)
				{
					if(!g_pCurrentBulletData->pEntity)
					{
						CPedGta *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
						if(*ppEntity == pLocalPed || (CVehicleGta*)ppEntity == pLocalPed->pVehicle)
						{
							*ppEntity = nullptr;
							vecPos->x = 0.0f;
							vecPos->y = 0.0f;
							vecPos->z = 0.0f;
							return 0;
						}
					}
				}
			}
		}

		if(g_pCurrentFiredPed)
		{
			if(g_pCurrentFiredPed == pGame->FindPlayerPed())
				SendBulletSync(vecOrigin, vecEnd, vecPos, (CEntityGta **)ppEntity);
		}

		return result;
	}

	return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}

signed int (*CBulletInfo_AddBullet)(CEntityGta* pEntity, CWeaponGta* pWeapon, CVector vec1, CVector vec2);
signed int CBulletInfo_AddBullet_hook(CEntityGta* pEntity, CWeaponGta* pWeapon, CVector vec1, CVector vec2)
{
	vec2.x *= 50.0f;
	vec2.y *= 50.0f;
	vec2.z *= 50.0f;
	CBulletInfo_AddBullet(pEntity, pWeapon, vec1, vec2);
	// CBulletInfo::Update
	(( void (*)())(g_libGTASA+0x55E170+1))();
	return 1;
}

bool (*CEventKnockOffBike__AffectsPed)(uintptr_t *thiz, CPedGta *a2);
bool CEventKnockOffBike__AffectsPed_hook(uintptr_t *thiz, CPedGta *a2)
{
	return false;
}

float *m_f3rdPersonCHairMultX, *m_f3rdPersonCHairMultY, *ms_fAspectRatio;

// Fixing a crosshair by very stupid math ( JPATCH )
static constexpr float ar43 = 4.0f / 3.0f;
void (*DrawCrosshair)(uintptr_t* thiz);
void DrawCrosshair_hook(uintptr_t* thiz)
{
	float save1 = *m_f3rdPersonCHairMultX;
	*m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;

	float save2 = *m_f3rdPersonCHairMultY;
	*m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;

	DrawCrosshair(thiz);

	*m_f3rdPersonCHairMultX = save1;
	*m_f3rdPersonCHairMultY = save2;
}

void (*CHud__DrawRadar)(uintptr_t* thiz);
void CHud__DrawRadar_hook(uintptr_t* thiz)
{
	if(CHUD::bIsShow)
	{
		float* radar = (float*) CGtaWidgets::pWidgets[0xA1];

		//thiz[0] = (float)100;
		if (radar)
		{

			radar[3] = CHUD::radarPos.X;
			radar[4] = CHUD::radarPos.Y;

			radar[5] = 38.0f;
			radar[6] = 38.0f;

//			thiz[5] = 45.0f;
//			thiz[6] = 45.0f;
		}
		CHud__DrawRadar(thiz);
	}

}

void InstallHooks()
{
	Log("InstallHooks");

	PROTECT_CODE_INSTALLHOOKS;

	//draw radar
	CHook::InlineHook(g_libGTASA, 0x00437B0C, &CHud__DrawRadar_hook, &CHud__DrawRadar);
//
//    // Fixing a crosshair by very stupid math ( JPATCH )
//	m_f3rdPersonCHairMultX = (float*)(g_libGTASA + 0x008B07FC);
//	m_f3rdPersonCHairMultY = (float*)(g_libGTASA + 0x008B07F8);
//	ms_fAspectRatio = (float*)(g_libGTASA + 0x0098525C);
//    CHook::InlineHook(g_libGTASA, 0x3D44CC, &DrawCrosshair_hook, &DrawCrosshair);
//
//	//
//	CHook::InlineHook(g_libGTASA, 0x003AC5DC, &FindPlayerPed_ab__hook, &FindPlayerPed_ab_); // crash fix?
//
//	//CHook::NOP(g_libGTASA + 0x50F3A0, 6); // CVehicle::ReduceVehicleDamage
	// не падать с мотоцикла
	CHook::InlineHook(g_libGTASA, 0x0037573C, &CEventKnockOffBike__AffectsPed_hook, &CEventKnockOffBike__AffectsPed);
//
//	// Стрельба
//	CHook::InlineHook(g_libGTASA, 0x564E28, &CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);
//	CHook::InlineHook(g_libGTASA, 0x55E090, &CBulletInfo_AddBullet_hook, &CBulletInfo_AddBullet);
//	CHook::InlineHook(g_libGTASA, 0x56668C, &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
//	CHook::InlineHook(g_libGTASA, 0x327528, &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
	CHook::InlineHook(g_libGTASA, 0x005DC178, &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
//	CHook::InlineHook(g_libGTASA, 0x3C70C0, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);
//
//	// audio
////	CHook::InlineHook(g_libGTASA, 0x368850, &CAudioEngine__Service_hook, &CAudioEngine__Service);
//	//CHook::InlineHook(g_libGTASA, 0x35AC44, &CAEVehicleAudioEntity__GetAccelAndBrake_hook, &CAEVehicleAudioEntity__GetAccelAndBrake);
//
	//CHook::InlineHook(g_libGTASA, 0x002DCA54, &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);
//	CHook::InlineHook(g_libGTASA, 0x5669D8, &CWeapon__GenerateDamageEvent_hook, &CWeapon__GenerateDamageEvent);
//	// Fire extingusher fix
//	CHook::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);
//
//	CHook::InlineHook(g_libGTASA, 0x0032217C, &CEventHandler__HandleEvents_hook, &CEventHandler__HandleEvents);
//	CHook::InlineHook(g_libGTASA, 0x00281398, &CWidgetRegionLook__Update_hook, &CWidgetRegionLook__Update);
//
	CHook::InlineHook(g_libGTASA, 0x0043AF28, &CLoadingScreen_DisplayPCScreen_hook, &CLoadingScreen_DisplayPCScreen);
//
	CHook::InlineHook(g_libGTASA, 0x003F646C, &Render2dStuff_hook, &Render2dStuff);
	CHook::InlineHook(g_libGTASA, 0x00269740, &TouchEvent_hook, &TouchEvent);
//    //
//

	CHook::InlineHook(g_libGTASA, 0x0044137C, &CRadar__GetRadarTraceColor_hook, &CRadar__GetRadarTraceColor); // dangerous
	CHook::InlineHook(g_libGTASA, 0x00442770, &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);
	CHook::InlineHook(g_libGTASA, 0x00443F30, &CRadar__DrawRadarGangOverlay_hook, &CRadar__DrawRadarGangOverlay);
//
//	CHook::CodeInject(g_libGTASA + 0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);
//
//

	CModelInfo::InstallHooks();

//
	//CHook::InlineHook(g_libGTASA, 0x003860DC, &CModelInfo_AddVehicleModel_hook, &CModelInfo_AddVehicleModel); // dangerous
//
//	CHook::InlineHook(g_libGTASA, 0x0033DA5C, &CAnimManager__UncompressAnimation_hook, &CAnimManager__UncompressAnimation);
//	CHook::InlineHook(g_libGTASA, 0x001AECC0, &RwFrameAddChild_hook, &RwFrameAddChild);
//	CHook::InlineHook(g_libGTASA, 0x002DFD30, &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
//	CHook::InlineHook(g_libGTASA, 0x0033AD78, &CAnimBlendNode__FindKeyFrame_hook, &CAnimBlendNode__FindKeyFrame);
//
	// attached objects
	CHook::InlineHook(g_libGTASA, 0x00428410, &CWorld_ProcessPedsAfterPreRender_hook, &CWorld_ProcessPedsAfterPreRender);
//
//	//remove building
//	//SetUpHook(g_libGTASA + 0x00395994, (uintptr_t)CFileLoader__LoadObjectInstance_hook, (uintptr_t*)& CFileLoader__LoadObjectInstance);
//
//	// retexture
//	//SetUpHook(g_libGTASA + 0x00391E20, (uintptr_t)CObject__Render_hook, (uintptr_t*)& CObject__Render);
//
//	// gettexture fix crash
	CHook::InlineHook(g_libGTASA, 0x00297280, &GetTexture_hook, &GetTexture_orig);
//
//	// steal objects fix
//	CHook::InlineHook(g_libGTASA, 0x003AD8E0, &CPlayerInfo__Process_hook, &CPlayerInfo__Process);
//
//	// GetFrameFromID fix
//	CHook::InlineHook(g_libGTASA, 0x00335CC0, &CClumpModelInfo_GetFrameFromId_hook, &CClumpModelInfo_GetFrameFromId);
//
//	// random crash fix
//	CHook::InlineHook(g_libGTASA, 0x001A8530, &RenderQueue__ProcessCommand_hook, &RenderQueue__ProcessCommand);
//	// fix
//	CHook::InlineHook(g_libGTASA, 0x001B9D74, &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);
//
//	//SetUpHook(g_libGTASA + 0x0027548C, (DWORD)CWidgetButtonAttack_hook, (DWORD*)&CWidgetButtonAttack);
//
//
//
//	//SetUpHook(g_libGTASA + 0x0027455C, (uintptr_t)CWidget__IsTouched_hook, (uintptr_t*)& CWidget__IsTouched);
//
//	CHook::RET(g_libGTASA + 0x002C0304); // CDarkel__RegisterCarBlownUpByPlayer_hook
//	CHook::RET(g_libGTASA + 0x002C072C); // CDarkel__ResetModelsKilledByPlayer_hook
//	CHook::RET(g_libGTASA + 0x002C0758); // CDarkel__QueryModelsKilledByPlayer_hook
//	CHook::RET(g_libGTASA + 0x002C0778); // CDarkel__FindTotalPedsKilledByPlayer_hook
//	CHook::RET(g_libGTASA + 0x002C0D04); // CDarkel__RegisterKillByPlayer_hook
//
//	CHook::InlineHook(g_libGTASA, 0x338CBC, &CVehicleModelInfo__SetEditableMaterials_hook, &CVehicleModelInfo__SetEditableMaterials);
//	CHook::InlineHook(g_libGTASA, 0x0050DEF4, &CVehicle__ResetAfterRender_hook, &CVehicle__ResetAfterRender);
//
	//CHook::InlineHook(g_libGTASA, 0x003F4000, &CGame__Process_hook, &CGame__Process);
//
//	CHook::InlineHook(g_libGTASA, 0x004D4A6C, &CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);
//
//	CHook::InlineHook(g_libGTASA, 0x0025CB8C, &MainMenuScreen__OnExit_hook, &MainMenuScreen__OnExit);
////	CHook::InlineHook(g_libGTASA, 0x00398334, &CGame__Shutdown_hook, &CGame__Shutdown);
//
//	CHook::WriteMemory(g_libGTASA + 0x003DA86C,
//		"\x80\xB4"\
//		"\x00\xAF"\
//		"\x1B\x4B"\
//		"\x7B\x44"\
//		"\x1B\x68", 10); // CRadar::LimitRadarPoint
//
//	CHook::NOP(g_libGTASA + 0x003DA876, 3); // CRadar::LimitRadarPoint
//
//	// headlights color, wheel size, wheel align
//	CHook::InlineHook(g_libGTASA, 0x005466EC, &CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
//	CHook::InlineHook(g_libGTASA, 0x00518EC4, &CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);
//
//	CHook::InlineHook(g_libGTASA, 0x004E671C, &CAutomobile__PreRender_hook, &CAutomobile__PreRender);
//	CHook::InlineHook(g_libGTASA, 0x004DC6E8, &CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
//	CHook::InlineHook(g_libGTASA, 0x003E8D48, &CMatrix__Rotate_hook, &CMatrix__Rotate);
//	CHook::InlineHook(g_libGTASA, 0x003E8884, &CMatrix__SetScale_hook, &CMatrix__SetScale);
//
//	CHook::InlineHook(g_libGTASA, 0x0046CEF4, &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);
//
//	CHook::InlineHook(g_libGTASA, 0x003DA86C, &CRadar__LimitRadarPoint_hook, &CRadar__LimitRadarPoint); // TO FIX
//	//CHook::InlineHook(g_libGTASA, 0x005529FC, &CSprite2d__DrawBarChart_hook, &CSprite2d__DrawBarChart);
//	//SetUpHook(g_libGTASA + 0x005353B4, (uintptr_t)CFont__PrintString_hook, (uintptr_t*)& CFont__PrintString);
	//CHook::InlineHook(g_libGTASA, 0x005C9170, &CSprite2d__Draw_hook, &CSprite2d__Draw);
//
//	//SetUpHook(g_libGTASA + 0x0027D8A8, (uintptr_t)CWidgetPlayerInfo__DrawWanted_hook, (uintptr_t*)& CWidgetPlayerInfo__DrawWanted);
//
//	//SetUpHook(g_libGTASA + 0x0027CE88, (uintptr_t)CWidgetPlayerInfo__DrawWeaponIcon_hook, (uintptr_t*)& CWidgetPlayerInfo__DrawWeaponIcon);
//	CHook::InlineHook(g_libGTASA, 0x00389D74, (uintptr_t)CCam__Process_hook, (uintptr_t*)& CCam__Process);
//
//	CHook::InlineHook(g_libGTASA, 0x003D6E6C, (uintptr_t)CHud__Draw_hook, (uintptr_t*)& CHud__Draw);
//	CHook::InlineHook(g_libGTASA, 0x0039DC68, (uintptr_t)CPad__CycleCameraModeDownJustDown_hook, (uintptr_t*)& CPad__CycleCameraModeDownJustDown);
//
//	CHook::InlineHook(g_libGTASA, 0x0031B164, (uintptr_t)FxEmitterBP_c__Render_hook, (uintptr_t*)& FxEmitterBP_c__Render);
	CHook::InlineHook(g_libGTASA, 0x004A339C, (uintptr_t)CPed__ProcessEntityCollision_hook, (uintptr_t*)&CPed__ProcessEntityCollision);
//	CHook::InlineHook(g_libGTASA, 0x3AA3C0, (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);
//
//	CHook::InlineHook(g_libGTASA, 0x004B97FC, &CTaskSimpleGetUp__ProcessPed_hook, &CTaskSimpleGetUp__ProcessPed); // CTaskSimpleGetUp::ProcessPed
//
//	//==================
//	CHook::InlineHook(g_libGTASA, 0x194B04, &emu_ArraysDelete_hook, &emu_ArraysDelete);
//	CHook::InlineHook(g_libGTASA, 0x1BA580, &RwResourcesFreeResEntry_hook, &RwResourcesFreeResEntry);
//	CHook::InlineHook(g_libGTASA, 0x194B20, &emu_ArraysGetID_hook, &emu_ArraysGetID);
//	CHook::InlineHook(g_libGTASA, 0x50C5F8, &SetCompAlphaCB_hook, &SetCompAlphaCB);
//
//	//================================
//	// ================== plaka ======
//	//================================
//
//	// vehicle light processing
//	CHook::InlineHook(g_libGTASA, 0x5189C4, &CVehicle__GetVehicleLightsStatus_hook, &CVehicle__GetVehicleLightsStatus);
//	CHook::NOP(g_libGTASA + 0x408AAA, 2);
//
//	//RpMaterialDestroy fix ? не точно
//	//CHook::InlineHook(g_libGTASA, 0x001E3C54, &RpMaterialDestroy_hook, &RpMaterialDestroy);
//	//CHook::InlineHook(g_libGTASA, 0x1B1808, &_RwTextureDestroy_hook, &_RwTextureDestroy);
//
//	//CRunningScript fix ? не точно
//	//CHook::InlineHook(g_libGTASA, 0x002E5400, &GivePedScriptedTask_hook, &GivePedScriptedTask);
//
//	// Настройки
//	CHook::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
//	CHook::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
//	CHook::NOP(g_libGTASA + 0x261A50, 2); // Main - Language
//	CHook::NOP(g_libGTASA + 0x2665EE, 2); // Game - SocialClub
//
//	//
////	CHook::InlineHook(g_libGTASA, 0x004904AC, &CTaskSimpleCarSetPedInAsDriver__ProcessPed_hook, &CTaskSimpleCarSetPedInAsDriver__ProcessPed);
//
//	// fix разрешения экрана
//	CHook::InlineHook(g_libGTASA, 0x0026CE30, &MobileSettings__GetMaxResWidth_hook, &MobileSettings__GetMaxResWidth);
//
//	//
//	//CHook::InlineHook(g_libGTASA, 0x39AE28, &RenderEffects_hook, &RenderEffects);
//
//	//размытие на скорости
//	CHook::InlineHook(g_libGTASA, 0x005311D0, &CDraw__SetFOV_hook, &CDraw__SetFOV);
//
//	//
	//CHook::InlineHook(g_libGTASA, 0x0040E870, &CEntity__RegisterReference_hook, &CEntity__RegisterReference);
//
	HookCPad();
}

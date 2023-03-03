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
#define MAX_ENCRYPTED_TXD 5
const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = {
        cryptor::create("texdb/txd/txd.txt", 19),
        cryptor::create("texdb/gta3/gta3.txt", 21),
        cryptor::create("texdb/gta_int/gta_int.txt", 27),
		cryptor::create("texdb/cars/cars.txt", 27),
		cryptor::create("texdb/skins/skins.txt", 27),
};

extern bool g_bIsTestMode;

bool isEncrypted(const char *szArch)
{
 //   if(g_bIsTestMode)return false;
	//return false;
    for (const auto & i : encrArch)
    {
        if (!strcmp(i.decrypt(), szArch))
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

	auto *st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE *f  = fopen(path, "rb");
	if(f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		return nullptr;
	}
}
#include "keyboard.h"
void Render2dStuff()
{
	if (pGUI) pGUI->Render();

	if(!CKeyBoard::m_bEnable)
		( ( void(*)(bool) )(g_libGTASA + 0x002B0BD8 + 1) )(false); // render widgets

	( ( void(*)() )(g_libGTASA + 0x00437200 + 1) )(); // прицел


	if (CHUD::bIsShow) {
		CHUD::UpdateHudInfo();

		float* radar = (float*) CGtaWidgets::pWidgets[0xA1];

		//thiz[0] = (float)100;
		if (radar)
		{

			radar[3] = CHUD::radarPos.X;
			radar[4] = CHUD::radarPos.Y;

			radar[5] = 38.0f;
			radar[6] = 38.0f;
		}

		( ( void(*)() )(g_libGTASA + 0x00437B0C + 1) )(); // радар
	}

	MainLoop();
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

	// cutscene
//	CHook::NOP(g_libGTASA + 0x0046F5B0, 2); // cutscene
//	CHook::RET(g_libGTASA + 0x0038FE64); // CCutsceneMgr::LoadCutsceneData
//
//	CHook::NOP(g_libGTASA + 0x0046F5C6, 2); // player
	CHook::NOP(g_libGTASA + 0x0046F5D2, 2); // menu

	TextureDatabaseRuntime::Load("samp", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);

	TextureDatabaseRuntime::Load("mobile", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("txd", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("gta3", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("gta_int", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Load("menu", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_PVR);

	//skins
	TextureDatabase* skins = TextureDatabaseRuntime::Load("skins", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Register(skins);
////
////	// cars
	TextureDatabase* cars = TextureDatabaseRuntime::Load("cars", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Register(cars);

	InitialiseRenderWare();
}

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
	CStreaming::AddImageToList("TEXDB\\SKINS.IMG", true);
    CStreaming::AddImageToList("TEXDB\\CARS.IMG", true);
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

void (*cHandlingDataMgr__ConvertDataToGameUnits)(uintptr_t *thiz, tHandlingData* handling);
void cHandlingDataMgr__ConvertDataToGameUnits_hook(uintptr_t *thiz, tHandlingData* handling)
{

	int32_t iHandling = ((int(*)(uintptr_t*, const char*))(g_libGTASA + 0x00570D28 + 1))(thiz, &szLastBufferedName[0]);

    if(!CHandlingDefault::bIsSlotUsed[iHandling])
    {
        CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);
    }

	cHandlingDataMgr__ConvertDataToGameUnits(thiz, handling);
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

		CKeyBoard::Close();

		g_pJavaWrapper->SetPauseState(true);

		CSpeedometr::tempToggle(false);
	}

	*(bool*)(g_libGTASA + 0x0096B514) = true;
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

	*(bool*)(g_libGTASA + 0x0096B514) = false;
}

extern char g_iLastBlock[512];
unsigned int(*LoadFullTexture)(uintptr_t *thiz, unsigned int entryIndex);
unsigned int LoadFullTexture_hook(uintptr_t *thiz, unsigned int entryIndex)
{
	strcpy(g_iLastBlock, *(const char **)(*((DWORD *)thiz + 7) + 0x17 * entryIndex));

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


void InstallSpecialHooks()
{
	Log("InstallSpecialHooks");

	//CHook::InlineHook(g_libGTASA, 0x00267054, &FileSetPosition_hook, &FileSetPosition);
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
	// gettexture fix crash
	CHook::Redirect(g_libGTASA, 0x00297280, &CUtil::GetTexture);
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
    CHook::InlineHook(g_libGTASA, 0x002A36E2, &MenuItem_add_hook, &MenuItem_add);
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
				strcpy(nick, pPlayerPool->GetPlayerName(damagedid));

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
				if (pNetGame->GetPlayerPool()->m_pPlayers[i])
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

int (*RpMaterialDestroy)(RpMaterial* mat);
int RpMaterialDestroy_hook(RpMaterial* mat)
{
//	if(!a1 || !a2 || !a3 || !a4)return 1;
//
//	return RpMaterialDestroy(a1, a2, a3, a4);
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

void (*CVehicleModelInfo__SetEditableMaterials)(uintptr_t);
void CVehicleModelInfo__SetEditableMaterials_hook(uintptr_t clump)
{
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

void (*CVehicle__ResetAfterRender)(uintptr_t);
void CVehicle__ResetAfterRender_hook(uintptr_t thiz)
{

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

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
void (*CAutomobile__ProcessEntityCollision)(CVehicleGta* a1, CEntityGta* a2, int a3);
void CAutomobile__ProcessEntityCollision_hook(CVehicleGta* a1, CEntityGta* a2, int a3)
{

	g_usLastProcessedModelIndexAutomobile = a1->nModelIndex;
	g_iLastProcessedModelIndexAutoEnt = a2->nModelIndex;

	bool bReplace = false;
	void* pOld = nullptr;
	uint8_t* pColData = nullptr;

	uint16_t vehId = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(a1);
	CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehId);
	if (pVeh) {
		if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR) {
			pColData = GetCollisionDataFromModel(a1->nModelIndex);
			if (pColData && pVeh->m_pSuspensionLines) {
				if (*(void **) (pColData + 16)) {
					pOld = *(void **) (pColData + 16);
					*(void **) (pColData + 16) = pVeh->m_pSuspensionLines;
					bReplace = true;
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
			uint16_t vehid = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(vehicle);
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
			uint16_t vehid = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(vehicle);
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
	CVehicleModelInfo* pModelInfoStart = static_cast<CVehicleModelInfo *>(CModelInfo::ms_modelInfoPtrs[thiz->nModelIndex]);

	float fOldFront = pModelInfoStart->m_fWheelSizeFront;
	float fOldRear = pModelInfoStart->m_fWheelSizeRear;

    if (pNetGame->GetVehiclePool()) {
        uint16_t vehid = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(thiz);
        auto pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
        if (pVeh) {
            pVeh->ProcessWheelsOffset();
            g_pLastProcessedVehicleMatrix = pVeh;

            if (pVeh->m_bWheelSize)
			{
				pModelInfoStart->m_fWheelSizeFront = pVeh->m_fWheelSize;
				pModelInfoStart->m_fWheelSizeRear = pVeh->m_fWheelSize;
            }
            if (pVeh->m_bShadow && pVeh->m_Shadow.pTexture) {
                CVehicle__DoHeadLightReflectionTwin(pVeh, pVeh->m_pVehicle->mat);
            }
        }
    }

	CAutomobile__PreRender(thiz);

	pModelInfoStart->m_fWheelSizeFront = fOldFront;
	pModelInfoStart->m_fWheelSizeRear = fOldRear;

	g_pLastProcessedVehicleMatrix = nullptr;
	g_iLastProcessedWheelVehicle = -1;
}


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
	//float pOld = *(float*)(g_libGTASA + 0x00608558);
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
				//	*(float*)(g_libGTASA + 0x00608558) = 200.0f;
				}
			}
		}
	}

	CCam__Process(thiz);
	if (pVeh)
	{
		pVeh->SetMoveSpeedVector(vecSpeed);
		//*(float*)(g_libGTASA + 0x00608558) = pOld;
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
						*(uint32_t*)(g_libGTASA + 0x00951FA8 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x00951FA8 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x00951FA8 + 40) = 0;
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
						*(uint32_t*)(g_libGTASA + 0x00951FA8 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x00951FA8 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x00951FA8 + 40) = 0;
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

	if (pPed->bInVehicle)
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
//
void (*FxEmitterBP_c__Render)(uintptr_t* thiz, uintptr_t *camPtr, uint32_t txdHashKey, float brightness, int doHeatHaze);
void FxEmitterBP_c__Render_hook(uintptr_t* thiz, uintptr_t *camPtr, uint32_t txdHashKey, float brightness, int doHeatHaze)
{
	RwTexture* tex = (RwTexture *)(thiz + 0xC);
	uintptr_t* temp = *((uintptr_t**)thiz + 3);
	if(!thiz || !camPtr || !temp || !tex )
	{
		//char utf[255];
		//cp1251_to_utf8(utf, tex->name);
	//	Log("text fx = %s", tex->name);
	}

	FxEmitterBP_c__Render(thiz, camPtr, txdHashKey, brightness, doHeatHaze);
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

void (*CDraw__SetFOV)(float fFOV, bool isCinematic);
void CDraw__SetFOV_hook(float fFOV, bool isCinematic)
{
	float tmp = (float)((float)((float)(*(float *)(g_libGTASA + 0xA26A90) + -1.3333) * 11.0) / 0.44444) + fFOV;
	if(tmp > 100) tmp = 100.0;
	*(float *)(g_libGTASA + 0x006B1CB8) = tmp;
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
							for(const auto & pVehicle : pVehiclePool->m_pVehicles)
							{
								if(pVehicle == nullptr || pVehicle->m_pVehicle) continue;

								MATRIX4X4 vehicleMat, playerMat;
								pVehicle->GetMatrix(&vehicleMat);
								pPlayerPed->GetMatrix(&playerMat);

								float fSX = (vehicleMat.pos.x - playerMat.pos.x) * (vehicleMat.pos.x - playerMat.pos.x);
								float fSY = (vehicleMat.pos.y - playerMat.pos.y) * (vehicleMat.pos.y - playerMat.pos.y);
								float fSZ = (vehicleMat.pos.z - playerMat.pos.z) * (vehicleMat.pos.z - playerMat.pos.z);

								auto fDistance = sqrt(fSX + fSY + fSZ);

								if(fDistance <= 100.0f)
									*((unsigned char*)thiz + 13) |= 1;
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

	if(dwRetAddr == 0x005DC7F0+1 || dwRetAddr == 0x005DC620+1 || dwRetAddr == 0x005DD0B0+1 || dwRetAddr == 0x005DCD56+1)
		g_bForceWorldProcessLineOfSight = true;

	return CWeapon__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, pWeaponSlot, ppEntity2, b1, b2, b3, b4, b5, b6, b7);
}

uint32_t (*CWorld__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8);
uint32_t CWorld__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGta **ppEntity, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x5dd0b0 + 1 || g_bForceWorldProcessLineOfSight)
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
						if(*ppEntity == pLocalPed || (CVehicleGta*)ppEntity == (CVehicleGta*)pLocalPed->pVehicle)
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

		CHud__DrawRadar(thiz);
	}

}

void (*AddWaterSplashParticles)(CVehicleGta* thiz);
void AddWaterSplashParticles_hook(CVehicleGta* thiz)
{
	Log("AddWaterSplashParticles");
	AddWaterSplashParticles(thiz);
}

#include "../java_systems/LoadingScreen.h"

void InstallHooks()
{
	Log("InstallHooks");

//	CHook::InlineHook(g_libGTASA, 0x0058AE18, &AddWaterSplashParticles_hook, &AddWaterSplashParticles);

	//draw radar
	//CHook::InlineHook(g_libGTASA, 0x00437B0C, &CHud__DrawRadar_hook, &CHud__DrawRadar);
//
//    // Fixing a crosshair by very stupid math ( JPATCH )
//	m_f3rdPersonCHairMultX = (float*)(g_libGTASA + 0x008B07FC);
//	m_f3rdPersonCHairMultY = (float*)(g_libGTASA + 0x008B07F8);
//	ms_fAspectRatio = (float*)(g_libGTASA + 0x0098525C);
//    CHook::InlineHook(g_libGTASA, 0x3D44CC, &DrawCrosshair_hook, &DrawCrosshair);

	// не падать с мотоцикла
	CHook::InlineHook(g_libGTASA, 0x0037573C, &CEventKnockOffBike__AffectsPed_hook, &CEventKnockOffBike__AffectsPed);
//
//	// Стрельба
	CHook::InlineHook(g_libGTASA, 0x005DF748, &CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);
//	CHook::InlineHook(g_libGTASA, 0x55E090, &CBulletInfo_AddBullet_hook, &CBulletInfo_AddBullet);
//	CHook::InlineHook(g_libGTASA, 0x56668C, &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
	CHook::InlineHook(g_libGTASA, 0x00371B3C, &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
	CHook::InlineHook(g_libGTASA, 0x005DC178, &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
	CHook::InlineHook(g_libGTASA, 0x00424B94, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);
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
//
	CHook::Redirect(g_libGTASA, 0x0043AF78, &LoadingScreen::gtaLoadingScreenTick);
//
	CHook::Redirect(g_libGTASA, 0x003F646C, &Render2dStuff);

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

//	CHook::InlineHook(g_libGTASA, 0x0033DA5C, &CAnimManager__UncompressAnimation_hook, &CAnimManager__UncompressAnimation);
//	CHook::InlineHook(g_libGTASA, 0x001AECC0, &RwFrameAddChild_hook, &RwFrameAddChild);
//	CHook::InlineHook(g_libGTASA, 0x002DFD30, &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
//	CHook::InlineHook(g_libGTASA, 0x0033AD78, &CAnimBlendNode__FindKeyFrame_hook, &CAnimBlendNode__FindKeyFrame);
//
	// attached objects
	CHook::InlineHook(g_libGTASA, 0x00428410, &CWorld_ProcessPedsAfterPreRender_hook, &CWorld_ProcessPedsAfterPreRender);

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


	CHook::InlineHook(g_libGTASA, 0x0055BF10, &CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);

	CHook::InlineHook(g_libGTASA, 0x0029DEF8, &MainMenuScreen__OnExit_hook, &MainMenuScreen__OnExit);
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
	// размер колес
	CHook::InlineHook(g_libGTASA, 0x00559D44, &CAutomobile__PreRender_hook, &CAutomobile__PreRender);
	CHook::InlineHook(g_libGTASA, 0x00559534, &CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
//	CHook::InlineHook(g_libGTASA, 0x003E8D48, &CMatrix__Rotate_hook, &CMatrix__Rotate);
//	CHook::InlineHook(g_libGTASA, 0x003E8884, &CMatrix__SetScale_hook, &CMatrix__SetScale);
//
//	CHook::InlineHook(g_libGTASA, 0x0046CEF4, &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);
//
//	CHook::InlineHook(g_libGTASA, 0x003DA86C, &CRadar__LimitRadarPoint_hook, &CRadar__LimitRadarPoint); // TO FIX


	//CHook::InlineHook(g_libGTASA, 0x005C9170, &CSprite2d__Draw_hook, &CSprite2d__Draw);

	CHook::InlineHook(g_libGTASA, 0x003BF300, (uintptr_t)CCam__Process_hook, (uintptr_t*)& CCam__Process);

	CHook::InlineHook(g_libGTASA, 0x003FB430, (uintptr_t)CPad__CycleCameraModeDownJustDown_hook, (uintptr_t*)& CPad__CycleCameraModeDownJustDown);
//
//	CHook::InlineHook(g_libGTASA, 0x0031B164, (uintptr_t)FxEmitterBP_c__Render_hook, (uintptr_t*)& FxEmitterBP_c__Render);
	CHook::InlineHook(g_libGTASA, 0x004A339C, (uintptr_t)CPed__ProcessEntityCollision_hook, (uintptr_t*)&CPed__ProcessEntityCollision);
//	CHook::InlineHook(g_libGTASA, 0x3AA3C0, (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);

//	//================================
//	// ================== plaka ======
//	//================================
//
//	// vehicle light processing
//	CHook::InlineHook(g_libGTASA, 0x5189C4, &CVehicle__GetVehicleLightsStatus_hook, &CVehicle__GetVehicleLightsStatus);
//	CHook::NOP(g_libGTASA + 0x408AAA, 2);
//
//	// Настройки
//	CHook::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
//	CHook::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
//	CHook::NOP(g_libGTASA + 0x261A50, 2); // Main - Language
//	CHook::NOP(g_libGTASA + 0x2665EE, 2); // Game - SocialClub
//

//	//CHook::InlineHook(g_libGTASA, 0x39AE28, &RenderEffects_hook, &RenderEffects);

	//размытие на скорости
	CHook::InlineHook(g_libGTASA, 0x005A61F4, &CDraw__SetFOV_hook, &CDraw__SetFOV);

	//CHook::InlineHook(g_libGTASA, 0x0040E870, &CEntity__RegisterReference_hook, &CEntity__RegisterReference);
//
	HookCPad();
}

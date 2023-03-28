#include "../main.h"
#include "RW/RenderWare.h"
#include <sstream>
#include "game.h"
#include "game/Enums/ePedState.h"

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
#include "../CSkyBox.h"

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
    if(g_bIsTestMode)return false;
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
		sprintf(path, "%smain.scm", g_pszStorage);
		Log("Loading %s", path);
	}
	// ----------------------------
	if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sscript.img", g_pszStorage);
		Log("Loading script.img..");
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
float *ms_fAspectRatio;
static constexpr float ar43 = 4.0f / 3.0f;

#include "keyboard.h"
void Render2dStuff()
{

	( ( void(*)(bool) )(g_libGTASA + 0x001C0750 + 1) )(false); // emu_GammaSet

	if (pGUI) pGUI->Render();

	// jpatch fix crosshair
	float saveX = *CCamera::m_f3rdPersonCHairMultX;
	*CCamera::m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;

	float saveY = *CCamera::m_f3rdPersonCHairMultY;
	*CCamera::m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;

	( ( void(*)() )(g_libGTASA + 0x00437200 + 1) )(); // прицел

	*CCamera::m_f3rdPersonCHairMultX = saveX;
	*CCamera::m_f3rdPersonCHairMultY = saveY;
	//

	if(!CKeyBoard::m_bEnable)
		( ( void(*)(bool) )(g_libGTASA + 0x002B0BD8 + 1) )(false); // render widgets


	if (CHUD::bIsShow) {
		CHUD::UpdateHudInfo();

		auto* radar = (float*) CGtaWidgets::pWidgets[0xA1];

		//thiz[0] = (float)100;
		if (radar)
		{

			radar[3] = CHUD::radarPos.x;
			radar[4] = CHUD::radarPos.y;

			radar[5] = 38.0f;
			radar[6] = 38.0f;
		}

		( ( void(*)() )(g_libGTASA + 0x00437B0C + 1) )(); // радар
	}

//	((void (*)(bool)) (g_libGTASA + 0x0054BDD4 + 1))(true); // gametext
//	((void (*)(bool)) (g_libGTASA + 0x005A9120 + 1))(true); // CFont::RenderFontBuffer


	MainLoop();
}

// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{


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

	TextureDatabaseRuntime::Load("samp", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);

	TextureDatabaseRuntime::Load("mobile", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("txd", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("gta3", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("gta_int", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);
	TextureDatabaseRuntime::Load("menu", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_DXT);

	//skins
	TextureDatabase* skins = TextureDatabaseRuntime::Load("skins", false, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);
	TextureDatabaseRuntime::Register(skins);

    // cars
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

#include "game/Models/ModelInfo.h"


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
		pPickups->PickedUp( ((dwParam1-(g_libGTASA+0x007AFD70))/0x20) );
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

void cHandlingDataMgr__ConvertDataToGameUnits(uintptr_t *thiz, tHandlingData* handling)
{

	int32_t iHandling = ((int(*)(uintptr_t*, const char*))(g_libGTASA + 0x00570D28 + 1))(thiz, &szLastBufferedName[0]);

    if(!CHandlingDefault::bIsSlotUsed[iHandling])
    {
        CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);
    }

	(( void (*)(uintptr_t*, tHandlingData* handling))(g_libGTASA + 0x00570DC8 + 1))(thiz, handling);
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

	if (isEncrypted(a3))
	{
		lastOpenedFile = *a2;
	}

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

int (*mpg123_param)(void* mh, int key, long val, int ZERO, double fval);
int mpg123_param_hook(void* mh, int key, long val, int ZERO, double fval) {
	// 0x2000 = MPG123_SKIP_ID3V2
	// 0x200  = MPG123_FUZZY
	// 0x100  = MPG123_SEEKBUFFER
	// 0x40   = MPG123_GAPLESS
	return mpg123_param(mh, key, val | (0x2000 | 0x200 | 0x100 | 0x40), ZERO, fval);
}

void InstallSpecialHooks()
{
	Log("InstallSpecialHooks");

	// Just a fuzzy seek. Tell MPG123 to not load useless data
	CHook::InlineHook(g_libGTASA, 0x0022F268, &mpg123_param_hook, &mpg123_param);

	// texture Краш если с текстурами какая-то хуйня
	CHook::InlineHook(g_libGTASA, 0x001E8FF8, &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);

	//IMG
	 CHook::InlineHook(g_libGTASA, 0x002CF680, &CStreaming_InitImageList_hook, (uintptr_t*)&CStreaming_InitImageList);

	//new fix
	//CHook::InlineHook(g_libGTASA, 0x00222C50, &rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);
	//CHook::InlineHook(g_libGTASA, 0x002CA85C, &CustomPipeRenderCB_hook, &CustomPipeRenderCB);

	// gettexture fix crash
	CHook::Redirect(g_libGTASA, 0x00297280, &CUtil::GetTexture);

	// CPlaceable::InitMatrixArray
	CHook::InlineHook(g_libGTASA, 0x00408B20, &CPlaceable_InitMatrixArray_hook, &CPlaceable_InitMatrixArray);

	// RLEDecompress fix
	//CHook::InlineHook(g_libGTASA, 0x001E7316, &RLEDecompress_hook, &RLEDecompress);

	// Log texture load
	CHook::InlineHook(g_libGTASA, 0x001E9148, &LoadFullTexture_hook, &LoadFullTexture);



	//pvr
	CHook::UnFuck(g_libGTASA + 0x001E87A0);
	*(char*)(g_libGTASA + 0x1E87A0 + 12) = 'd';
	*(char*)(g_libGTASA + 0x1E87A0 + 13) = 'x';
	*(char*)(g_libGTASA + 0x1E87A0 + 14) = 't';

	CHook::UnFuck(g_libGTASA + 0x001E8C04);
	*(char*)(g_libGTASA + 0x001E8C04 + 12) = 'd';
	*(char*)(g_libGTASA + 0x001E8C04 + 13) = 'x';
	*(char*)(g_libGTASA + 0x001E8C04 + 14) = 't';

	//etc
	CHook::UnFuck(g_libGTASA + 0x001E878C);
	*(char*)(g_libGTASA + 0x001E878C + 12) = 'd';
	*(char*)(g_libGTASA + 0x001E878C + 13) = 'x';
	*(char*)(g_libGTASA + 0x001E878C + 14) = 't';

	CHook::UnFuck(g_libGTASA + 0x001E8BF4);
	*(char*)(g_libGTASA + 0x001E8BF4 + 12) = 'd';
	*(char*)(g_libGTASA + 0x001E8BF4 + 13) = 'x';
	*(char*)(g_libGTASA + 0x001E8BF4 + 14) = 't';

	CHook::InlineHook(g_libGTASA, 0x002671E2, &OS_FileRead_hook, &OS_FileRead);
	CHook::InlineHook(g_libGTASA, 0x00266DB4, &NvFOpen_hook, & NvFOpen);

	CHook::InlineHook(g_libGTASA, 0x0046F570, &InitialiseRenderWare_hook, &InitialiseRenderWare);
	CHook::InlineHook(g_libGTASA, 0x0029C06C, &MainMenuScreen__Update_hook, &MainMenuScreen__Update);
	CHook::InlineHook(g_libGTASA, 0x00266A68, &OS_FileOpen_hook, &OS_FileOpen);

	CHook::InlineHook(g_libGTASA, 0x00570C70, &cHandlingDataMgr__FindExactWord_hook, &cHandlingDataMgr__FindExactWord);
	CHook::MethodHook(g_libGTASA, 0x0067125C, &cHandlingDataMgr__ConvertDataToGameUnits);

	CHook::InlineHook(g_libGTASA, 0x0046BB04, &CStreaming__Init2_hook, &CStreaming__Init2);	// increase stream memory value
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
				strcpy(nick, pPlayerPool->GetPlayerName(damagedid));

				CHUD::addTakeDamageNotify(pPlayerPool->GetPlayerName(damagedid), weaponid, fDamage);
			}
		}
	}

	CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pEntity, pDamageResponse, bSpeak);
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

int g_iLastRenderedObject;

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

		pNewFrameResult = ((RwFrame * (*)(RpClump * pClump, int id))(g_libGTASA + 0x003856D0 + 1))(pClump, i);

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

#include "..//crashlytics.h"
#include "game/RW/rpworld.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;

char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, char* a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, char* a2)
{
	Log("%s", a2);
	return RenderQueue__ProcessCommand(thiz, a2);
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

std::list<std::pair<unsigned int*, unsigned int>> resetEntriesVehicle;


RpMaterial* CVehicle__SetupRenderMatCB(RpMaterial* material, void* data)
{
	if (material)
	{
		if (material->texture)
		{
			auto pVeh = (CVehicle*)data;

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
//			RwRGBA color = material->color;
//			int v11;
//			v11 = *(DWORD *)&color & 0xFFFFFF;
//			if(v11 == 0xFF3C)
//			{ // first color
//				resetEntriesVehicle.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&(material->color)), *reinterpret_cast<unsigned int*>(&(material->color))));
//				material->color.red = pVeh->color1.R;
//				material->color.green = pVeh->color1.G;
//				material->color.blue = pVeh->color1.B;
//               // Log("%d", pVeh->m_byteColor1);
//			}
		}
	}
	return material;
}

uintptr_t CVehicle__SetupRenderCB(uintptr_t atomic, void* data)
{
	if (*(RpGeometry * *)(atomic + 24))
	{
		((RpGeometry * (*)(RpGeometry*, uintptr_t, void*))(g_libGTASA + 0x00215F30 + 1))(*(RpGeometry * *)(atomic + 24), (uintptr_t)CVehicle__SetupRenderMatCB, (void*)data); // RpGeometryForAllMaterials
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
					((RpClump * (*)(RpClump *, uintptr_t, void *))(g_libGTASA + 0x00213D66 + 1))(pClump, (uintptr_t)CVehicle__SetupRenderCB, (void *)pVehicle); // RpClumpForAllAtomics
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
#include "Timer.h"

void (*CGame__Process)();
void CGame__Process_hook()
{
	if(pGame->bIsGameExiting)return;

    uint32_t CurrentTimeInCycles;
    uint32_t v1; // r4
    uint32_t v2; // r5
    uint32_t v3; // r5

	//FIXME
	((void(*)())(g_libGTASA + 0x003F8B50 + 1))(); // CPad::UpdatePads()
	((void(*)())(g_libGTASA + 0x002B03F8 + 1))(); // CTouchInterface::Clear()
	((void(*)())(g_libGTASA + 0x0028C178 + 1))(); // CHID::Update()

//	auto g_LoadMonitor = (uintptr_t *) (g_libGTASA + 0x009595EC);
//	((void(*)(uintptr_t*))(g_libGTASA + 0x003F4FB6 + 1))(g_LoadMonitor); // CLoadMonitor::BeginFrame

    CurrentTimeInCycles = CTimer::GetCurrentTimeInCycles();
    v1 = CurrentTimeInCycles / CTimer::GetCyclesPerMillisecond();

	((void(*)())(g_libGTASA + 0x002D0558 + 1))(); // CStreaming::Update()

    v2 = CTimer::GetCurrentTimeInCycles();
    v3 = v2 / CTimer::GetCyclesPerMillisecond();

	//((void(*)())(g_libGTASA + 0x00390120 + 1))(); // CCutsceneMgr::Update();

	if ( !(CTimer::m_CodePause << 0x18) )
	{
		auto gMobileMenu = (uintptr_t *) (g_libGTASA + 0x006E0074);
		((void(*)(uintptr_t*))(g_libGTASA + 0x0029A730 + 1))(gMobileMenu); // MobileMenu::Update
	}

   // ((void(*)())(g_libGTASA + 0x0042CF54 + 1))(); // CTheZones::Update()
    ((void(*)())(g_libGTASA + 0x004D4F00 + 1))(); // CCover::Update()

	auto TheCamera = (uintptr_t *) (g_libGTASA + 0x00951FA8);

//	auto p_tx = (CSimpleTransform *)&TheCamera + 0x14 + 0x30;
//	if ( !TheCamera.m_pMat )
//		p_tx = *TheCamera + 0x4;

	//CAudioZones::Update(0, p_tx->m_translate);

	*(int32_t*)(g_libGTASA + 0x00A7D22C) = 0; // CWindModifiers::Number

    if ( !CTimer::m_CodePause && !CTimer::m_UserPause )
    {
		((void (*)()) (g_libGTASA + 0x005C8884 + 1))(); // CSprite2d::SetRecipNearClip();
		((void (*)()) (g_libGTASA + 0x005C89F8 + 1))(); // CSprite2d::InitPerFrame();
		((void (*)()) (g_libGTASA + 0x005A8A74 + 1))(); // CFont::InitPerFrame()
//		((void (*)()) (g_libGTASA + 0x002FEB2C + 1))(); // CCheat::DoCheats();
//		((void (*)()) (g_libGTASA + 0x003E33C8 + 1))(); // CClock::Update()

        ((void (*)()) (g_libGTASA + 0x005CC2E8 + 1))(); // CWeather::Update()
        ((void(*)())(g_libGTASA + 0x0032AED8 + 1))(); // CTheScripts::Process()
        ((void(*)())(g_libGTASA + 0x002D9818 + 1))(); // CCollision::Update()

//		auto ThePaths = (uintptr_t *) (g_libGTASA + 0x007A61F0);
//		((void (*)(uintptr_t *)) (g_libGTASA + 0x003195F4 + 1))(ThePaths); // CPathFind::UpdateStreaming

//		CTrain::UpdateTrains();
//		CHeli::UpdateHelis();
		//((void(*)())(g_libGTASA + 0x003036B4 + 1))(); // CDarkel::Update()
		((void(*)())(g_libGTASA + 0x005BE838 + 1))(); // CSkidmarks::Update();
		((void(*)())(g_libGTASA + 0x005AB4C8 + 1))(); // CGlass::Update()
//		CWanted::UpdateEachFrame();
//		((void(*)())(g_libGTASA + 0x003F3640 + 1))(); // CCreepingFire::Update();
		((void(*)())(g_libGTASA + 0x0035E4AC + 1))(); // CSetPieces::Update();

        auto gFireManager = (uintptr_t *) (g_libGTASA + 0x00958800);
        ((void (*)(uintptr_t *)) (g_libGTASA + 0x003F1628 + 1))(gFireManager); // CFireManager::Update


		((void(*)(bool))(g_libGTASA + 0x004CC380 + 1))(false); // CPopulation::Update нужно (

		((void (*)()) (g_libGTASA + 0x005DB8E8 + 1))(); // CWeapon::UpdateWeapons()
        ((void (*)()) (g_libGTASA + 0x0059EB48 + 1))(); // CClouds::Update()
		((void (*)()) (g_libGTASA + 0x005A6720 + 1))(); // CMovingThings::Update();
		((void(*)())(g_libGTASA + 0x005CBB20 + 1))(); // CWaterCannons::Update()
		//((void (*)()) (g_libGTASA + 0x004213FC + 1))(); // CUserDisplay::Process()
        ((void (*)()) (g_libGTASA + 0x00427744 + 1))(); // CWorld::Process()

        if ( !CTimer::bSkipProcessThisFrame )
		{
            ((void (*)()) (g_libGTASA + 0x0059CFC0 + 1))(); // CBirds::Update()
            ((void (*)()) (g_libGTASA + 0x005C03E4 + 1))(); // CSpecialFX::Update()
        }
//		auto g_InterestingEvents = *(uint8_t*)(g_libGTASA + 0x009EF9D8);
//		if ( *((uint8_t *)&g_InterestingEvents + 0x12C) << 0x1F )
//			((void (*)(uint8_t*)) (g_libGTASA + 0x004C1EC0 + 1))(&g_InterestingEvents); // CInterestingEvents::ScanForNearbyEntities

        ((void (*)()) (g_libGTASA + 0x005B28D8 + 1))(); // CPostEffects::Update()
        ((void (*)()) (g_libGTASA + 0x0041EF78 + 1))(); // CTimeCycle::Update()
        ((void (*)()) (g_libGTASA + 0x004CA834 + 1))(); // CPopCycle::Update()

        ((void (*)(uintptr_t *)) (g_libGTASA + 0x003DC7D0 + 1))(TheCamera); // CCamera::Process()

		//((void (*)()) (g_libGTASA + 0x005CF8D0 + 1))(); // CCullZones::Update()
		//((void (*)()) (g_libGTASA + 0x00307D8C + 1))(); // CGameLogic::Update()
		//CGangWars::Update();
		//((void (*)()) (g_libGTASA + 0x003006C4 + 1))(); // CConversations::Update()
		//((void (*)()) (g_libGTASA + 0x00301B68 + 1))(); // CPedToPlayerConversations::Update()
		//((void (*)()) (g_libGTASA + 0x002E388E + 1))(); // CBridge::Update()
        ((void (*)()) (g_libGTASA + 0x005A3E40 + 1))(); // CCoronas::DoSunAndMoon()
        ((void (*)()) (g_libGTASA + 0x005A22C8 + 1))(); // CCoronas::Update()
		((void (*)()) (g_libGTASA + 0x005BD370 + 1))(); // CShadows::UpdatePermanentShadows()
		((void (*)()) (g_libGTASA + 0x002CA3A4 + 1))(); // CCustomBuildingRenderer::Update()

		//
//		auto p_m_transform = (CSimpleTransform *)&TheCamera + 0x14 + 0x30;
//		if ( ! (uintptr_t*)(&TheCamera + 0x14) )
//			p_m_transform = (CSimpleTransform *)(&TheCamera + 0x4);

	//	((void (*)(CSimpleTransform*)) (g_libGTASA + 0x002CE120 + 1))(p_m_transform); // CPlantMgr::Update
		//

		auto g_fx = *(uintptr_t *) (g_libGTASA + 0x00820520);
		auto temp = *(uintptr_t*)&TheCamera + 0x8D8;
		((void (*)(uintptr_t*, uintptr_t, float )) (g_libGTASA + 0x00363DE0 + 1))(&g_fx, temp, CTimer::ms_fTimeStep / 50.0f); // Fx_c::Update

		auto g_breakMan = (uintptr_t *) (g_libGTASA + 0x0099DD14);
		((void (*)(uintptr_t*, float )) (g_libGTASA + 0x0045267C + 1))(g_breakMan, CTimer::ms_fTimeStep); // BreakManager_c::Update

		auto g_procObjMan = (uintptr_t *) (g_libGTASA + 0x009A1F3C);
		((void (*)(uintptr_t* )) (g_libGTASA + 0x004571A0 + 1))(g_procObjMan); // ProcObjectMan_c::Update

		auto g_waterCreatureMan = (uintptr_t *) (g_libGTASA + 0x00A14350);
		((void (*)(uintptr_t*, float )) (g_libGTASA + 0x004571A0 + 1))(g_waterCreatureMan, CTimer::ms_fTimeStep / 50.0f); // WaterCreatureManager_c::Update

		((void (*)()) (g_libGTASA + 0x00596540 + 1))(); // CWaterLevel::PreRenderWater()
	}
//

	static bool once = false;
	if (!once)
	{
		CCustomPlateManager::Initialise();
		once = true;
	}

	CCustomPlateManager::Process();

//	CGame__Process();
}

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
int32_t (*CAutomobile__ProcessEntityCollision)(CVehicleGta* a1, CEntityGta* a2, uintptr_t* aColPoints);
int32_t CAutomobile__ProcessEntityCollision_hook(CVehicleGta* a1, CEntityGta* a2, uintptr_t* aColPoints)
{
	int32_t res;

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

	res = CAutomobile__ProcessEntityCollision(a1, a2, aColPoints);
	if (bReplace)
	{
		*(void**)(pColData + 16) = pOld;
	}
	return res;
}

void (*MainMenuScreen__OnExit)();
void MainMenuScreen__OnExit_hook()
{
	*(bool*)(g_libGTASA + 0x009FC8FC + 0x10) = true; // RsGlobal.quit

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

void CVehicle__DoHeadLightReflectionTwin(CVehicle* pVeh, CMatrix* matVehicle)
{
	CVehicleGta* v2; // r4
	int v3; // r2
	CMatrix* v4; // r5
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
	v4 = matVehicle;
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

void (*CVehicle__DoHeadLightBeam)(CVehicleGta* vehicle, int32_t lightId, RwMatrix* matrix, bool isRight);
void CVehicle__DoHeadLightBeam_hook(CVehicleGta* vehicle, int32_t lightId, RwMatrix* matrix, bool isRight)
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

	CVehicle__DoHeadLightBeam(vehicle, lightId, matrix, isRight);

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

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

int32_t (*CPed__ProcessEntityCollision)(CPedGta* thiz, CEntityGta* ent, void* colPoint);
int32_t CPed__ProcessEntityCollision_hook(CPedGta* thiz, CEntityGta* ent, void* colPoint)
{
	g_iLastProcessedSkinCollision = thiz->nModelIndex;
	g_iLastProcessedEntityCollision = ent->nModelIndex;

	return CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

void (*CDraw__SetFOV)(float fFOV, bool isCinematic);
void CDraw__SetFOV_hook(float fFOV, bool isCinematic)
{
	float tmp = (float)((float)((float)(*(float *)(g_libGTASA + 0xA26A90) + -1.3333) * 11.0) / 0.44444) + fFOV;
	if(tmp > 100) tmp = 100.0;
	*(float *)(g_libGTASA + 0x006B1CB8) = tmp;
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

								RwMatrix vehicleMat, playerMat;
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

bool CVehicle__GetVehicleLightsStatus(CVehicleGta *_pVehicle)
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

    CVehicle *pVehicle = pVehiclePool->FindVehicle(_pVehicle);
	if(!pVehicle)return false;

	return pVehicle->m_bLightsOn;
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
					static RwMatrix mat1;
					memset(&mat1, 0, sizeof(mat1));

					static RwMatrix mat2;
					memset(&mat2, 0, sizeof(mat2));

					RwMatrixOrthoNormalize(&mat2, pEntity->mat->ToRwMatrix());
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
							auto pMatrix = g_pCurrentBulletData->pEntity->mat;
							if(pMatrix)
							{
								if(pNetGame->m_iLagCompensation)
								{
									vecPosPlusOffset.x = pMatrix->pos.x + g_pCurrentBulletData->vecOffset.x;
									vecPosPlusOffset.y = pMatrix->pos.y + g_pCurrentBulletData->vecOffset.y;
									vecPosPlusOffset.z = pMatrix->pos.z + g_pCurrentBulletData->vecOffset.z;
								}
								else ProjectMatrix(&vecPosPlusOffset, pMatrix->ToRwMatrix(), &g_pCurrentBulletData->vecOffset);

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
	(( void (*)())(g_libGTASA+0x005D7094+1))();
	return 1;
}

bool (*CEventKnockOffBike__AffectsPed)(uintptr_t *thiz, CPedGta *a2);
bool CEventKnockOffBike__AffectsPed_hook(uintptr_t *thiz, CPedGta *a2)
{
	return false;
}



uintptr_t* (*rpMaterialListDeinitialize)(RpMaterialList* matList);
uintptr_t* rpMaterialListDeinitialize_hook(RpMaterialList* matList)
{
	if(!matList || !matList->materials)
		return nullptr;

	return rpMaterialListDeinitialize(matList);
}

uintptr_t* (*ConvertBufferToObject)(uint8_t* fileBuffer, int32_t modelId);
uintptr_t* ConvertBufferToObject_hook(uint8_t* fileBuffer, int32_t modelId)
{
	Log("ConvertBufferToObject = %d", modelId);

	return ConvertBufferToObject(fileBuffer, modelId);
}

#include "../java_systems/LoadingScreen.h"

void (*CRenderer__RenderEverythingBarRoads)();
void CRenderer__RenderEverythingBarRoads_hook()
{
	CSkyBox::Process();

	CRenderer__RenderEverythingBarRoads();
}

void(*CObject__Render)(CEntityGta*);
void CObject__Render_hook(CEntityGta* thiz)
{
	// 0051ABB0 + 1
	// 004353FE + 1
	// 004352C4 + 1

	if (CSkyBox::GetSkyObject())
	{
		if (CSkyBox::GetSkyObject()->m_pEntity == thiz && !CSkyBox::IsNeedRender())
			return;
	}

	CObject__Render(thiz);
}

#include "Scene.h"

void InstallHooks()
{
	Log("InstallHooks");

	Scene = *(CScene*)(g_libGTASA + 0x009FC938);
    CTimer::InjectHooks();

	//CHook::InlineHook(g_libGTASA, 0x002D2FD0, &ConvertBufferToObject_hook, &ConvertBufferToObject);

    // Fixing a crosshair by very stupid math ( JPATCH )
	ms_fAspectRatio = (float*)(g_libGTASA + 0x00A26A90);

	// skybox
//	CHook::InlineHook(g_libGTASA, 0x00454F54, &CObject__Render_hook, &CObject__Render);
//	CHook::InlineHook(g_libGTASA, 0x0040FD64, &CRenderer__RenderEverythingBarRoads_hook, &CRenderer__RenderEverythingBarRoads);

	// не падать с мотоцикла
	CHook::InlineHook(g_libGTASA, 0x0037573C, &CEventKnockOffBike__AffectsPed_hook, &CEventKnockOffBike__AffectsPed);

	// Стрельба
	CHook::InlineHook(g_libGTASA, 0x005DF748, &CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);
	CHook::InlineHook(g_libGTASA, 0x55E090, &CBulletInfo_AddBullet_hook, &CBulletInfo_AddBullet);
	CHook::InlineHook(g_libGTASA, 0x56668C, &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
	CHook::InlineHook(g_libGTASA, 0x00371B3C, &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
	CHook::InlineHook(g_libGTASA, 0x005DC178, &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
	CHook::InlineHook(g_libGTASA, 0x00424B94, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);

	//CHook::InlineHook(g_libGTASA, 0x002DCA54, &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);
//	CHook::InlineHook(g_libGTASA, 0x5669D8, &CWeapon__GenerateDamageEvent_hook, &CWeapon__GenerateDamageEvent);
//	// Fire extingusher fix
//	CHook::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);
//
//	CHook::InlineHook(g_libGTASA, 0x0032217C, &CEventHandler__HandleEvents_hook, &CEventHandler__HandleEvents);

	CHook::Redirect(g_libGTASA, 0x0043AF78, &LoadingScreen::gtaLoadingScreenTick);
//
	CHook::Redirect(g_libGTASA, 0x003F646C, &Render2dStuff);

	CHook::InlineHook(g_libGTASA, 0x00269740, &TouchEvent_hook, &TouchEvent);

	CHook::InlineHook(g_libGTASA, 0x0044137C, &CRadar__GetRadarTraceColor_hook, &CRadar__GetRadarTraceColor); // dangerous
	CHook::InlineHook(g_libGTASA, 0x00442770, &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);
	CHook::InlineHook(g_libGTASA, 0x00443F30, &CRadar__DrawRadarGangOverlay_hook, &CRadar__DrawRadarGangOverlay);
//
//	CHook::CodeInject(g_libGTASA + 0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);

	CModelInfo::InstallHooks();

	// attached objects
	CHook::InlineHook(g_libGTASA, 0x00428410, &CWorld_ProcessPedsAfterPreRender_hook, &CWorld_ProcessPedsAfterPreRender);

	// crash
	CHook::InlineHook(g_libGTASA, 0x002176EC, &rpMaterialListDeinitialize_hook, &rpMaterialListDeinitialize);

	// GetFrameFromID fix
//	CHook::InlineHook(g_libGTASA, 0x00335CC0, &CClumpModelInfo_GetFrameFromId_hook, &CClumpModelInfo_GetFrameFromId);

	// random crash fix
//	CHook::InlineHook(g_libGTASA, 0x001D1F7E, &RenderQueue__ProcessCommand_hook, &RenderQueue__ProcessCommand);

	// fix
//	CHook::InlineHook(g_libGTASA, 0x001B9D74, &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);

	CHook::InlineHook(g_libGTASA, 0x00388B54, &CVehicleModelInfo__SetEditableMaterials_hook, &CVehicleModelInfo__SetEditableMaterials);
	CHook::InlineHook(g_libGTASA, 0x005823F0, &CVehicle__ResetAfterRender_hook, &CVehicle__ResetAfterRender);
//
	CHook::InlineHook(g_libGTASA, 0x003F4000, &CGame__Process_hook, &CGame__Process);

	CHook::InlineHook(g_libGTASA, 0x0055BF10, &CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);

	CHook::InlineHook(g_libGTASA, 0x0029DEF8, &MainMenuScreen__OnExit_hook, &MainMenuScreen__OnExit);

	// headlights color, wheel size, wheel align
//	CHook::InlineHook(g_libGTASA, 0x005B9D38, &CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
//	CHook::InlineHook(g_libGTASA, 0x00518EC4, &CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);
//
	// размер колес
	CHook::InlineHook(g_libGTASA, 0x00559D44, &CAutomobile__PreRender_hook, &CAutomobile__PreRender);
	CHook::InlineHook(g_libGTASA, 0x00559534, &CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
//	CHook::InlineHook(g_libGTASA, 0x003E8D48, &CMatrix__Rotate_hook, &CMatrix__Rotate);
	CHook::InlineHook(g_libGTASA, 0x0044EFBE, &CMatrix__SetScale_hook, &CMatrix__SetScale);

//	CHook::InlineHook(g_libGTASA, 0x003BF300, (uintptr_t)CCam__Process_hook, (uintptr_t*)& CCam__Process);

	CHook::InlineHook(g_libGTASA, 0x003FB430, (uintptr_t)CPad__CycleCameraModeDownJustDown_hook, (uintptr_t*)& CPad__CycleCameraModeDownJustDown);

	CHook::InlineHook(g_libGTASA, 0x004A339C, (uintptr_t)CPed__ProcessEntityCollision_hook, (uintptr_t*)&CPed__ProcessEntityCollision);

//	//================================
//	// ================== plaka ======
//	//================================
//
	// vehicle light processing
	CHook::Redirect(g_libGTASA, 0x00590230, &CVehicle__GetVehicleLightsStatus);

	// Настройки
//	CHook::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
//	CHook::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
//	CHook::NOP(g_libGTASA + 0x261A50, 2); // Main - Language
	CHook::NOP(g_libGTASA + 0x002A4A62, 2); // Game - SocialClub


	//размытие на скорости
	CHook::InlineHook(g_libGTASA, 0x005A61F4, &CDraw__SetFOV_hook, &CDraw__SetFOV);

	HookCPad();
}

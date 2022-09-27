#include "../main.h"
#include "../util/armhook.h"
#include "common.h"
#include "../CSettings.h"
#include "..//CDebugInfo.h"
#include "game.h"
#include "CExtendedCarColors.h"
#include "../util/patch.h"

extern CSettings* pSettings;

char* PLAYERS_REALLOC = nullptr;

int test_pointsArray[1000];
int test_pointersLibArray[1000];

void DisableAutoAim()
{
	// CPlayerPed::FindWeaponLockOnTarget
	CPatch::WriteMemory(g_libGTASA + 0x4568B0, "\x00\x20\xF7\x46", 4);
	// CPlayerPed::FindNextWeaponLockOnTarget
	CPatch::WriteMemory(g_libGTASA + 0x4590E4, "\x00\x20\xF7\x46", 4);
	CPatch::WriteMemory(g_libGTASA + 0x438DB4, "\x00\x20\xF7\x46", 4);
	
}

void ApplyPatches_level0()
{
	CPatch::WriteMemory(g_libGTASA + 0x573640, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x573668, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x57367C, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x573690, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x5736C8, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x5736D8, (int)"dxt", 3);
  	CPatch::WriteMemory(g_libGTASA + 0x5736E8, (int)"dxt", 3);

	PLAYERS_REALLOC = ((char* (*)(uint32_t))(g_libGTASA + 0x179B40))(404 * 257 * sizeof(char));
	memset(PLAYERS_REALLOC, 0, 404 * 257);
	CPatch::UnFuck(g_libGTASA + 0x5D021C);
	*(char**)(g_libGTASA + 0x5D021C) = PLAYERS_REALLOC;

	CPatch::RET(g_libGTASA + 0x463870);	// C_PcSave::SaveSlot
	DisableAutoAim();

	// Settings
	CPatch::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
    CPatch::NOP(g_libGTASA + 0x2661DA, 2); // Display - Shadows
    CPatch::NOP(g_libGTASA + 0x2665EE, 2); // Game - SocialClub

	// 3 touch begin
	CPatch::UnFuck(g_libGTASA + 0x005D1E8C);
	memset(test_pointsArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x005D1E8C) = &test_pointsArray[0];

	CPatch::UnFuck(g_libGTASA + 0x0063D4F0);
	memset(test_pointersLibArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x0063D4F0) = &test_pointersLibArray[0];

	CPatch::WriteMemory(g_libGTASA + 0x00238232, "\x03\x20", 2);
	CPatch::WriteMemory(g_libGTASA + 0x0025C522, "\x02\x2C", 2);
	// 3 touch end

	// col links limits!!!
	CPatch::WriteMemory(g_libGTASA + 0x1859FC, "\x01\x22", 2);
	//sizeof(CCollisionLink) = 12 (0xC)
	// we need 200 col links(MALO LI!!!)

	CPatch::WriteMemory(g_libGTASA + 0x0029554A, "\x4f\xf4\x61\x60", 4); // allocate memory to 300 * sizeof(CCollisionLink)
	CPatch::WriteMemory(g_libGTASA + 0x00295556, "\x4f\xf4\x5b\x62", 4); // BUT MAKE INITIALIZED ONLY 292 DUE TO SHIT ARM ASM!! (292 * sizeof(CCollisionLink)

	// col links limits end

	CExtendedCarColors::ApplyPatches_level0();

	uint8_t fps = 90;
	if (pSettings)
	{
		fps = pSettings->GetReadOnly().iFPS;
	}
	CPatch::WriteMemory(g_libGTASA + 0x463FE8, & fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C1F6, & fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C126, & fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x95B074, & fps, 1);

	//CPatch::UnFuck(g_libGTASA + 0x0056CA68);
	//*(uint8_t*)(g_libGTASA + 0x0056CA68) = 1;

	//CPatch::NOP(g_libGTASA + 0x001A869C, 2);

	// CPatch::NOP random pickups
	CPatch::NOP(g_libGTASA + 0x00402472, 2);
	CPatch::NOP(g_libGTASA + 0x003E1AF0, 2);

	// osMutexStuff
	CPatch::WriteMemory(g_libGTASA + 0x001A7ECE, "\x4F\xF0\x01\x00\x00\x46", 6);

	// CdStreamInit(6);
	CPatch::WriteMemory(g_libGTASA + 0x3981EC, "\x06\x20", 2);

	// fix hook crash
	CPatch::WriteMemory(g_libGTASA + 0x00336618, "\x4F\xF4\x6A\x71", 4);
	CPatch::WriteMemory(g_libGTASA + 0x33661C, "\x1A\x4B", 2);
	CPatch::WriteMemory(g_libGTASA + 0x00274AB4, "\x00\x46", 2);

	//CPatch::WriteMemory(g_libGTASA + 0x001A7EB6, "\x00\x23", 2); // MOVS            R3, #0
	//CPatch::WriteMemory(g_libGTASA + 0x001A7ED2, "\x00\x20", 2); // MOVS				R0, #0
}

uint32_t* CStreaming__msFiles;

void ApplyPatches()
{
	Log("Installing patches..");

	CDebugInfo::ApplyDebugPatches();

	// CPatch::NOP check CWorld::FindPlayerSlotWithPedPointer (fuck*** R*)
	// just left mov r4, r0 and ldr r1 [pc, shit]
	CPatch::NOP(g_libGTASA + 0x0045A4C8, 11);
	CPatch::NOP(g_libGTASA + 0x0045A4E0, 3);

	//CPatch::UnFuck(g_libGTASA + 0x008B8018);
	//*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;

	// CAudioEngine::StartLoadingTune
	CPatch::NOP(g_libGTASA + 0x56C150, 2);
	// CPatch::NOP CFileLoader::loadPickups

	// mordor:
	// sub_45A32(dword_14A458 + 0x284BB8, nullsub_39, 2);
	// sub_45A32(dword_14A458 + 0x320F2C, nullsub_39, 2);
	// sub_459F2(dword_14A458 + 0x39899A, 2);
	/*
		sub_45A32(dword_14A458 + 0x3D4244, nullsub_39, 2);
  sub_45A32(dword_14A458 + 0x3D42A8, nullsub_39, 2);
  sub_45A32(dword_14A458 + 0x2DC8E0, nullsub_39, 2);	
	*/

	CPatch::NOP(g_libGTASA + 0x402472, 2);
	// fucking shit from rockstar CRealTimeShadowManager::Update(void)
	CPatch::NOP(g_libGTASA + 0x0039B2C0, 2);
	// DefaultPCSaveFileName
	char* DefaultPCSaveFileName = (char*)(g_libGTASA + 0x60EAE8);
	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);

	uint8_t fps = pSettings->GetReadOnly().iFPS;
	CPatch::WriteMemory(g_libGTASA + 0x463FE8, & fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C1F6, & fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C126, & fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x95B074, & fps, 1);

	// CWidgetRegionSteeringSelection::Draw
	CPatch::WriteMemory(g_libGTASA + 0x284BB8, "\xF7\x46", 2);

	// CHud::SetHelpMessage
	CPatch::WriteMemory(g_libGTASA + 0x3D4244, "\xF7\x46", 2);
	// CHud::SetHelpMessageStatUpdate
	CPatch::WriteMemory(g_libGTASA + 0x3D42A8, "\xF7\x46", 2);
	// CVehicleRecording::Load
	CPatch::WriteMemory(g_libGTASA + 0x2DC8E0, "\xF7\x46", 2);

	CPatch::WriteMemory(g_libGTASA + 0x00458D68, "\x00\x46\x00\x46", 4); // cFire::extinguish CPatch::NOPped

	//CPatch::WriteMemory(g_libGTASA + 0x00541BF8, "\xD3", 1);
	//CPatch::WriteMemory(g_libGTASA + 0x00541CA4, "\xD3", 1);
	//CPatch::WriteMemory(g_libGTASA + 0x00541CF6, "\x59\xF8\x04\x0C", 4);
	//00541CF6

	// CPatch::NOP calling CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
	CPatch::NOP(g_libGTASA + 0x3A019C, 2);

	// Fix shadows crash (thx War Drum Studios)
	CPatch::UnFuck(g_libGTASA + 0x0039B2C4);
	CPatch::NOP(g_libGTASA + 0x0039B2C4, 2);

	/*
	todo:
	CPatch::NOP 482680
	
	CPatch::NOP 002C3B82
	
	*/

	CPatch::UnFuck(g_libGTASA + 0x00454950);
	CPatch::NOP(g_libGTASA + 0x00454950, 17); // CAnimManager::RemoveAnimBlockRef

	// CPed pool (old: 140, new: 210)
	/* 	MOVW R0, #0x5EC8
		MOVT R0, #6 */
	CPatch::WriteMemory(g_libGTASA + 0x3AF2D0, "\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x65EC8 | size=0x7C4 (old: 0x43F30)
	CPatch::WriteMemory(g_libGTASA + 0x3AF2DE, "\xD2\x20", 2); // MOVS R0, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF2E4, "\xD2\x22", 2); // MOVS R2, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF310, "\xD2\x2B", 2); // CMP  R3, #0xD2

	// CPedIntelligence pool (old: 140, new: 210)
	// movw r0, #0x20B0
	// movt r0, #2
	// CPatch::NOP
	CPatch::WriteMemory(g_libGTASA + 0x3AF7E6, "\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x220B0 | size=0x298 (old: 0x16B20)
	CPatch::WriteMemory(g_libGTASA + 0x3AF7F6, "\xD2\x20", 2); // MOVS R0, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF7FC, "\xD2\x22", 2); // MOVS R2, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF824, "\xD2\x2B", 2); // CMP  R3, 0xD2

	// Task pool (old: 500, new: 1524 (1536))
	CPatch::WriteMemory(g_libGTASA + 0x3AF4EA, "\x4F\xF4\x40\x30", 4); // MOV.W R0, #30000 | size = 0x80 (old: 0xFA00)
	CPatch::WriteMemory(g_libGTASA + 0x3AF4F4, "\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
	CPatch::WriteMemory(g_libGTASA + 0x3AF4FC, "\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
	CPatch::WriteMemory(g_libGTASA + 0x3AF52A, "\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600

	// TxdStore pool (old: 5000, new: 20000)
	CPatch::WriteMemory(g_libGTASA + 0x0055BA9A, "\x4f\xf4\xb8\x50\xc0\xf2\x11\x00", 8); //  MOV.W	R0, #0x1700 | MOVT	R0, #0x11
	CPatch::WriteMemory(g_libGTASA + 0x0055BAA8, "\x44\xf6\x20\x60", 4); // MOVW	R0, #0x4E20
	CPatch::WriteMemory(g_libGTASA + 0x0055BAB0, "\x44\xf6\x20\x62", 4); // MOVW	R2, #0x4E20

	// Event pool (old: 200, new: 512)

	// Object pool (old: 350, new: 2992)
	CPatch::WriteMemory(g_libGTASA + 0x003AF3D6, "\x4f\xf4\x7e\x40\xc0\xf2\x12\x00", 8); // MOV.W	R0, #0x5900 | MOVT	R0, #0x6
	CPatch::WriteMemory(g_libGTASA + 0x003AF3E4, "\x4f\xf4\x3b\x60", 4); // MOV.W R0, #0xBB0
	CPatch::WriteMemory(g_libGTASA + 0x003AF3EC, "\x4f\xf4\x3b\x62", 4); // MOV.W R2, #0xBB0
	CPatch::WriteMemory(g_libGTASA + 0x003AF41A, "\xb3\xf5\x3b\x6f", 4); // CMP.W R3, #0xBB0

	// Building pool (old: 14000, new: 29585)
	CPatch::WriteMemory(g_libGTASA + 0x003AF378, "\x4F\xF4\x90\x40\xC0\xF2\x19\x00", 8); // mov.w r0, #0x4800 movt r0, #0x19
	CPatch::WriteMemory(g_libGTASA + 0x003AF386, "\x47\xF2\x91\x30", 4); // mov r0, #0x7391
	CPatch::WriteMemory(g_libGTASA + 0x003AF38E, "\x47\xF2\x91\x32", 4); //mov r2, #0x7391

	// Dummys pool (old: 3500, new: 4000)
	CPatch::WriteMemory(g_libGTASA + 0x003AF430, "\x4f\xf4\xd6\x40\xc0\xf2\x03\x00", 8); // MOV.W	R0, #0x6B00 | MOVT	R0, #0x3
	CPatch::WriteMemory(g_libGTASA + 0x003AF43E, "\x40\xf6\xa0\x70", 4); // MOVW            R0, #0xFA0
	CPatch::WriteMemory(g_libGTASA + 0x003AF446, "\x40\xf6\xa0\x72", 4); // MOVW            R2, #0xFA0

	// PtrNode Single pool (old: 75000, new: 100000)
	CPatch::WriteMemory(g_libGTASA + 0x003AF1BC, "\x4f\xf4\x54\x50\xc0\xf2\x0c\x00", 8); // MOV.W	R0, #0x3500 | MOVT	R0, #0xC
	CPatch::WriteMemory(g_libGTASA + 0x003AF1D6, "\x48\xf2\xa0\x62", 4); // MOVW	R2, #0x86A0
	CPatch::WriteMemory(g_libGTASA + 0x003AF1B0, "\x48\xf2\xa0\x66", 4); // MOVW	R6, #0x86A0
	CPatch::WriteMemory(g_libGTASA + 0x003AF1CA, "\x48\xf2\xa0\x60\xc0\xf2\x01\x00", 8); // MOV.W R0, #0x86A0 | MOVT R0, #1

	//PtrNode Double pool (old: 6000, new: 8000)
	CPatch::WriteMemory(g_libGTASA + 0x003AF21C, "\x4f\xf4\xee\x40\xc0\xf2\x01\x00", 8); // MOV.W	R0, #0x7700 | MOVT	R0, #1
	CPatch::WriteMemory(g_libGTASA + 0x003AF22A, "\x41\xf6\x40\x70", 4); // MOVW            R0, #8000
	CPatch::WriteMemory(g_libGTASA + 0x003AF232, "\x41\xf6\x40\x72", 4); // MOVW            R2, #8000

	// Entry Node Info pool (old: 500, new: 5120)
	CPatch::WriteMemory(g_libGTASA + 0x003AF27A, "\x4f\xf4\xc8\x30", 4); // MOV.W	R0, #0x19000 | size = 0x14
	CPatch::WriteMemory(g_libGTASA + 0x003AF284, "\x4f\xf4\xa0\x50", 4); // MOV.W R0, #0x1400
	CPatch::WriteMemory(g_libGTASA + 0x003AF28C, "\x4f\xf4\xa0\x52", 4); // MOV.W R2, #0x1400
	CPatch::WriteMemory(g_libGTASA + 0x003AF2BA, "\xb3\xf5\xa0\x5f", 4); // CMP.W R3, #0x1400

	// ColModel pool (old:10150, new: 32511)
	// mov r0, #0xCFD0
	// movt r0, #0x17
	CPatch::WriteMemory(g_libGTASA + 0x3AF48E, "\x4C\xF6\xD0\x70\xC0\xF2\x17\x00", 8); // MOV R0, #0x17CFD0 | size=0x30 (old: 0x76F20)
	CPatch::WriteMemory(g_libGTASA + 0x3AF49C, "\x47\xF6\xFF\x60", 4); // MOVW R0, #0x7EFF
	CPatch::WriteMemory(g_libGTASA + 0x3AF4A4, "\x47\xF6\xFF\x62", 4); // MOVW R2, #0x7EFF

	// VehicleStruct increase (0x32C*0x50 = 0xFDC0)
	CPatch::WriteMemory(g_libGTASA + 0x405338, "\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
	CPatch::WriteMemory(g_libGTASA + 0x405342, "\x50\x20", 2);			// MOVS R0, #0x50
	CPatch::WriteMemory(g_libGTASA + 0x405348, "\x50\x22", 2);			// MOVS R2, #0x50
	CPatch::WriteMemory(g_libGTASA + 0x405374, "\x50\x2B", 2);			// CMP  R3, #0x50

	// Increase matrix count in CPlaceable::InitMatrixArray 
	CPatch::WriteMemory(g_libGTASA + 0x3ABB0A, "\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000

	// stop assign 0xFFFFFF to beam color!
	CPatch::NOP(g_libGTASA + 0x00519116, 2);
	CPatch::NOP(g_libGTASA + 0x0051911C, 4);
	CPatch::NOP(g_libGTASA + 0x00519198, 4);
	CPatch::NOP(g_libGTASA + 0x00519198, 2);
	CPatch::NOP(g_libGTASA + 0x005191D0, 4);
	CPatch::NOP(g_libGTASA + 0x005191DC, 2);

	// Prevent another ped from answering when collision happens (or damage)
	CPatch::NOP(g_libGTASA + 0x327730, 2);
	CPatch::NOP(g_libGTASA + 0x32C488, 2);
	CPatch::NOP(g_libGTASA + 0x32DF94, 2);
	CPatch::NOP(g_libGTASA + 0x32E05E, 2);
	CPatch::NOP(g_libGTASA + 0x32E160, 2);
	CPatch::NOP(g_libGTASA + 0x32E1F2, 2);
	CPatch::NOP(g_libGTASA + 0x32E292, 2);
	CPatch::NOP(g_libGTASA + 0x32EFD0, 2);

	// CPed::Say
	CPatch::NOP(g_libGTASA + 0x43E288, 2);

	// Prevent double vehicle sound service
	CPatch::NOP(g_libGTASA + 0x4E31A6, 2);
	CPatch::NOP(g_libGTASA + 0x4EE7D2, 2);
	CPatch::NOP(g_libGTASA + 0x4F741E, 2);
	CPatch::NOP(g_libGTASA + 0x50AB4A, 2);

	// Disable in-game radio
	CPatch::NOP(g_libGTASA + 0x3688DA, 2);
	CPatch::NOP(g_libGTASA + 0x368DF0, 2);
	CPatch::NOP(g_libGTASA + 0x369072, 2);
	CPatch::NOP(g_libGTASA + 0x369168, 2);

	// Stop it trying to load tracks2.dat
	CPatch::NOP(g_libGTASA + 0x508F36, 2);

	// Stop it trying to load tracks4.dat
	CPatch::NOP(g_libGTASA + 0x508F54, 2);

	// Prevent cheats processing
	CPatch::NOP(g_libGTASA + 0x3987BA, 2);
}

void ApplyInGamePatches()
{
	
	Log("Installing patches (ingame)..");

	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + 0x8EA7B0), 1, 100);
	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA + 0x8EA7A8) = 100;

	// CTaskSimplePlayerOnFoot::PlayIdleAnimations 
	CPatch::WriteMemory(g_libGTASA + 0x4BDB18, "\x70\x47", 2);

	// CarCtl::GenerateRandomCars nulled from CGame::Process
	CPatch::UnFuck(g_libGTASA + 0x398A3A);
	CPatch::NOP(g_libGTASA + 0x398A3A, 2);

	// CTheCarGenerators::Process nulled from CGame::Process
	CPatch::UnFuck(g_libGTASA + 0x398A34);
	CPatch::NOP(g_libGTASA + 0x398A34, 2);

	// MaxHealth
	CPatch::UnFuck(g_libGTASA + 0x3BAC68);
	*(float*)(g_libGTASA + 0x3BAC68) = 176.0f;
	//  Armour
	CPatch::UnFuck(g_libGTASA + 0x27D884);
	*(float*)(g_libGTASA + 0x27D884) = 176.0;

	// CEntryExit::GenerateAmbientPeds nulled from CEntryExit::TransitionFinished
	CPatch::UnFuck(g_libGTASA + 0x2C2C22);
	CPatch::NOP(g_libGTASA + 0x2C2C22, 4);

	CPatch::NOP(g_libGTASA + 0x003D6FDC, 2);
	CPatch::NOP(g_libGTASA + 0x0026B214, 2);

	CPatch::WriteMemory(g_libGTASA + 0x3D62FC, "\xF7\x46", 2);

	// CPlayerPed::CPlayerPed task fix
	CPatch::WriteMemory(g_libGTASA + 0x458ED1, "\xE0", 1);

	// ReapplyPlayerAnimation
	CPatch::NOP(g_libGTASA + 0x45477E, 5);

	// radar draw blips
	CPatch::UnFuck(g_libGTASA + 0x3DCA90);
	CPatch::NOP(g_libGTASA + 0x3DCA90, 2);
	CPatch::UnFuck(g_libGTASA + 0x3DD4A4);
	CPatch::NOP(g_libGTASA + 0x3DD4A4, 2);
	// CCamera::CamShake from CExplosion::AddExplosion
	CPatch::NOP(g_libGTASA + 0x55EFB8, 2);
	CPatch::NOP(g_libGTASA + 0x55F8F8, 2);

	// camera_on_actor path
	CPatch::UnFuck(g_libGTASA + 0x2F7B68);
	*(uint8_t*)(g_libGTASA + 0x2F7B6B) = 0xBE;

	// CPed::RemoveWeaponWhenEnteringVehicle (GetPlayerInfoForThisPlayerPed)
	CPatch::UnFuck(g_libGTASA + 0x434D94);
	CPatch::NOP(g_libGTASA + 0x434D94, 6);

	// CBike::ProcessAI
	CPatch::UnFuck(g_libGTASA + 0x4EE200);
	*(uint8_t*)(g_libGTASA + 0x4EE200) = 0x9B;

	// CWidgetPlayerInfo::DrawWanted
	CPatch::WriteMemory(g_libGTASA + 0x27D8D0, "\x4F\xF0\x00\x08", 4);

	// no vehicle audio processing
	CPatch::UnFuck(g_libGTASA + 0x4E31A6);
	CPatch::NOP(g_libGTASA + 0x4E31A6, 2);
	CPatch::UnFuck(g_libGTASA + 0x4EE7D2);
	CPatch::NOP(g_libGTASA + 0x4EE7D2, 2);
	CPatch::UnFuck(g_libGTASA + 0x4F741E);
	CPatch::NOP(g_libGTASA + 0x4F741E, 2);
	CPatch::UnFuck(g_libGTASA + 0x50AB4A);
	CPatch::NOP(g_libGTASA + 0x50AB4A, 2);

	CPatch::NOP(g_libGTASA + 0x00398768, 2); // CPatch::NOP police helis
	CPatch::NOP(g_libGTASA + 0x003987DC, 2);

	CPatch::NOP(g_libGTASA + 0x003688EC, 2); // CPatch::NOP ServiceAmbientGunFire

	//todo CPlayerPed::ProcessAnimGroups in the end
}
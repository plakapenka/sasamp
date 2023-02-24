#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "util/patch.h"
#include "CWorld.h"

#include <jni.h>

extern CGame *pGame;
extern CNetGame *pNetGame;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uintptr_t dwCurPlayerActor = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteCurDriver = 0;
int16_t(*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
int16_t CPad__GetPedWalkLeftRight_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return (int16_t)RemotePlayerKeys[byteCurPlayer].wKeyLR;;
	}
	else
	{
		int16_t dwResult = CPad__GetPedWalkLeftRight(thiz);
		LocalPlayerKeys.wKeyLR = (uint16_t)dwResult;
		return dwResult;
	}
}

int16_t(*CPad__GetPedWalkUpDown)(uintptr_t thiz);
int16_t CPad__GetPedWalkUpDown_hook(uintptr_t thiz)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return (int16_t)RemotePlayerKeys[byteCurPlayer].wKeyUD;;
	}
	else
	{
		// Local player
		int16_t dwResult = CPad__GetPedWalkUpDown(thiz);
		LocalPlayerKeys.wKeyUD = (uint16_t)dwResult;
		return dwResult;
	}
}

int (*CPad__GetLookLeft)(uintptr_t *thiz, bool a2);
int CPad__GetLookLeft_hook(uintptr_t *thiz, bool a2)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__GetLookLeft(thiz, a2);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
}

int (*CPad__GetLookRight)(uintptr_t *thiz, bool a2);
int CPad__GetLookRight_hook(uintptr_t *thiz, bool a2)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__GetLookRight(thiz, a2);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
}

bool (*CPad__GetTurretRight)(uintptr_t *thiz);
bool CPad__GetTurretRight_hook(uintptr_t *thiz)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__GetTurretRight(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
	}
}

bool (*CPad__GetTurretLeft)(uintptr_t *thiz);
bool CPad__GetTurretLeft_hook(uintptr_t *thiz)
{
	if(byteCurDriver != 0)
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__GetTurretLeft(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
	}
}

//int (*CPad__ShiftTargetLeftJustDown)(uintptr_t *thiz);
//int CPad__ShiftTargetLeftJustDown_hook(uintptr_t *thiz)
//{
//	Log("shitft L = %d", CPad__ShiftTargetLeftJustDown(thiz));
//	if(dwCurPlayerActor && (byteCurPlayer != 0))
//	{
//		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_LEFT];
//	}
//	else
//	{
//		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT] = CPad__ShiftTargetLeftJustDown(thiz);
//		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT];
//	}
//}
//
//int (*CPad__ShiftTargetRightJustDown)(uintptr_t *thiz);
//int CPad__ShiftTargetRightJustDown_hook(uintptr_t *thiz)
//{
//	Log("shitft R = %d", CPad__ShiftTargetRightJustDown(thiz));
//	if(dwCurPlayerActor && (byteCurPlayer != 0))
//	{
//		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_LOOK_RIGHT];
//	}
//	else
//	{
//		LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT] = CPad__ShiftTargetRightJustDown(thiz);
//		return LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT];
//	}
//}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyLR;
	}
	else
	{
		int16_t dwResult = CPad__GetSteeringLeftRight(thiz);
		LocalPlayerKeys.wKeyLR = (uint16_t)dwResult;
		return dwResult;
	}
}

int16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
int16_t CPad__GetSteeringUpDown_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyUD;
	}
	else
	{
		// local player
		int16_t dwResult = CPad__GetSteeringUpDown(thiz);
		LocalPlayerKeys.wKeyUD = (uint16_t)dwResult;
		return dwResult;
	}
}

uint32_t (*CPad__SprintJustDown)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__SprintJustDown_hook(uintptr_t thiz, uint32_t unk)
{
	return 0;
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_hook(uintptr_t thiz, uint32_t unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t (*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			!RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		if(pGame->isBanJump)
		{
			return 0;
		}
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
        if(pGame->isBanJump)
        {
            return 0;
        }

		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAbortClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return 0;
	}
	else
	{
		return CPad__DuckJustDown(thiz, unk);
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK])
			return 2;

		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		//LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = true;

		//if(dwResult == 2) 
		//{
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
		//}
		//else if(dwResult == 1)
		//{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;
		//	LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = false;
		//}

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle() || pPlayerPed->IsAPassenger())
				return 0;
		}

		// local player
		uint16_t wAccelerate = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
		if (wAccelerate == 0xFF)
		{
			if (pPlayerPed)
			{
				CVehicleGta* pGtaVehicle = pPlayerPed->GetGtaVehicle();
				if (pGtaVehicle)
				{
					if (pGtaVehicle->m_nVehicleFlags.bEngineOn == 0)
					{
						wAccelerate = 0;
					}
				}
			}
		}

		return wAccelerate;
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint32_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

uint32_t (*CPad__GetHorn)(uintptr_t* thiz);
uint32_t CPad__GetHorn_hook(uintptr_t* thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (!pPlayerPed->IsInVehicle())
				return 0;
		}
		// local player
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = pGame->isHornActive;
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
	//return (*CPad__GetHorn)(thiz);

}

uint32_t (*CVehicle__UsesSiren)(uintptr_t* thiz);
uint32_t CVehicle__UsesSiren_hook(uintptr_t* thiz)
{
	return 0;
}

extern float * pfCameraExtZoom;
void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_hook(uintptr_t thiz)
{
	dwCurPlayerActor = thiz;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// REMOTE PLAYER
		uint16_t byteSavedCameraMode;
		byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);

		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);

		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4)* wCameraMode2 = 0;

		// CPed::UpdatePosition nulled from CPed::ProcessControl
		//CHook::NOP(g_libGTASA + 0x004A2A92, 2);

		CWorld::PlayerInFocus = byteCurPlayer;
		// call original
		
		CPed__ProcessControl(thiz);
		// restore
		//CHook::WriteMemory(g_libGTASA + 0x004A2A92, "\xFA\xF7\x1D\xF8", 4);
		CWorld::PlayerInFocus = 0;
		*pbyteCameraMode = byteSavedCameraMode;

		GameSetLocalPlayerCameraExtZoom();
		GameSetLocalPlayerAim();
		*wCameraMode2 = wSavedCameraMode2;
	}
	else
	{
		// LOCAL PLAYER

		//CHook::WriteMemory(g_libGTASA + 0x4BED92, "\x10\x60", 2);
		
		(*CPed__ProcessControl)(thiz);

		// Reapply the no ped rots from Cam patch
		//CHook::WriteMemory(g_libGTASA + 0x4BED92, "\x00\x46", 2);
	}

	return;
}

void AllVehicles__ProcessControl_hook(uintptr_t thiz)
{
	CVehicleGta *pVehicle = (CVehicleGta*)thiz;
	uintptr_t this_vtable = pVehicle->vtable;
	this_vtable -= g_libGTASA;

	uintptr_t call_addr = 0;

	switch(this_vtable)
	{
		// CAutomobile
		case 0x0066D6A4:
		call_addr = 0x00553E44;
		break;

		// CBoat
		case 0x0066DA4C:
		call_addr = 0x0056BEC0;
		break;

		// CBike
		case 0x0066D81C:
		call_addr = 0x00561A90;
		break;

		// CPlane
		case 0x0066DDB0:
		call_addr = 0x00575CF8;
		break;

		// CHeli
		case 0x0066DB60:
		call_addr = 0x005712A8;
		break;

		// CBmx
		case 0x0066D934:
		call_addr = 0x00568B84;
		break;

		// CMonsterTruck
		case 0x0066DC88:
		call_addr = 0x00574864;
		break;

		// CQuadBike
		case 0x0066DED8:
		call_addr = 0x0057A2F0;
		break;

		// CTrain
		case 0x0066E128:
		call_addr = 0x0057D0A0;
		break;
	}

	if(pVehicle && pVehicle->pDriver)
	{
		byteCurDriver = FindPlayerNumFromPedPtr((uintptr_t)pVehicle->pDriver);
	}

	if(pVehicle->pDriver && pVehicle->pDriver->dwPedType == 0 &&
		pVehicle->pDriver != GamePool_FindPlayerPed() && 
		CWorld::PlayerInFocus == 0) // CWorld::PlayerInFocus
	{
		CWorld::PlayerInFocus = 0;

	//	pVehicle->pDriver->dwPedType = 4;
		//CAEVehicleAudioEntity::Service
	//	(( void (*)(uintptr_t))(g_libGTASA+0x003ACE04+1))(thiz+0x138);
	//	pVehicle->pDriver->dwPedType = 0;
	}
	else
	{
	//	(( void (*)(uintptr_t))(g_libGTASA+0x003ACE04+1))(thiz+0x138);
	}

	// Tyre burst fix
	if (pVehicle->pDriver)
	{
		if (pVehicle->m_nVehicleFlags.bTyresDontBurst)
		{
			pVehicle->m_nVehicleFlags.bTyresDontBurst = 0;
		}
		if(!pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = true;
	}
	else
	{
		if (!pVehicle->m_nVehicleFlags.bTyresDontBurst)
		{
			pVehicle->m_nVehicleFlags.bTyresDontBurst = 1;
		}
		if (pVehicle->m_nVehicleFlags.bCanBeDamaged) pVehicle->m_nVehicleFlags.bCanBeDamaged = false;
	}

	// VEHTYPE::ProcessControl()
    (( void (*)(CVehicleGta*))(g_libGTASA + call_addr + 1))(pVehicle);
}


uint32_t(*CPad__GetWeapon)(uintptr_t thiz, uintptr_t ped, bool unk);
uint32_t CPad__GetWeapon_hook(uintptr_t thiz, uintptr_t ped, bool unk)
{
	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint8_t old = CWorld::PlayerInFocus;
		CWorld::PlayerInFocus = byteCurPlayer;
		uintptr_t value = CPad__GetWeapon(thiz, ped, unk);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = value;
		CWorld::PlayerInFocus = old;
		return value;
	}
}

#include "gui/gui.h"


uintptr_t g_playerPed;

uintptr_t(*ProcessPlayerWeapon)(uintptr_t thiz, uintptr_t playerPed);
uintptr_t ProcessPlayerWeapon_hook(uintptr_t thiz, uintptr_t playerPed)
{
	g_playerPed = playerPed;
	uintptr_t toRetn = ProcessPlayerWeapon(thiz, playerPed);

	g_playerPed = 0;
	return toRetn;
}

uint32_t(*GetTarget)(uintptr_t thiz, int a2);
uint32_t GetTarget_hook(uintptr_t thiz, int a2)
{
	if (!g_playerPed)
	{
		return GetTarget(thiz, a2);
	}
	if (!(*(uint32_t*)(g_playerPed + 1088)))
	{
		return GetTarget(thiz, a2);
	}

	return *(uint8_t*)(*(uint32_t*)(g_playerPed + 1088) + 52) & 0b00001000;
}

uint32_t(*CPad__GetEnterTargeting)(uintptr_t thiz);
uint32_t CPad__GetEnterTargeting_hook(uintptr_t thiz)
{

	if (dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE];
	}
	else
	{
		if(pNetGame && pNetGame->m_GreenZoneState )
		{
			return 0;
		}
		uint8_t old = CWorld::PlayerInFocus;
		CWorld::PlayerInFocus = byteCurPlayer;
		uintptr_t result = CPad__GetEnterTargeting(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = result;


		CWorld::PlayerInFocus = old;
		return result;
	}
}

uint32_t TaskUseGun(uintptr_t thiz, uintptr_t ped)
{
	dwCurPlayerActor = ped;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	uint32_t result = 0;

	if (dwCurPlayerActor &&
		(byteCurPlayer != 0)) // not local player and local player's keys set.
	{
		uint16_t byteSavedCameraMode = *pbyteCameraMode;
		*pbyteCameraMode = GameGetPlayerCameraMode(byteCurPlayer);

		uint16_t wSavedCameraMode2 = *wCameraMode2;
		*wCameraMode2 = GameGetPlayerCameraMode(byteCurPlayer);
		if (*wCameraMode2 == 4)* wCameraMode2 = 0;

		// save the camera zoom factor, apply the context
		GameStoreLocalPlayerCameraExtZoom();
		GameSetRemotePlayerCameraExtZoom(byteCurPlayer);

		// aim switching
		GameStoreLocalPlayerAim();
		GameSetRemotePlayerAim(byteCurPlayer);
		CWorld::PlayerInFocus = byteCurPlayer;

		result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x004DDB70 + 1))(thiz, ped);

		// restore the camera modes, internal id and local player's aim
		*pbyteCameraMode = byteSavedCameraMode;

		// remote the local player's camera zoom factor
		GameSetLocalPlayerCameraExtZoom();

		CWorld::PlayerInFocus = 0;
		GameSetLocalPlayerAim();
		*wCameraMode2 = wSavedCameraMode2;
	}
	else
	{
		result = ((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x004DDB70 + 1))(thiz, ped);
	}

	return result;
}


// CTaskSimplePlayerOnFoot::ProcessPed
bool CPad__TaskProcess(uintptr_t *thiz, uintptr_t pPed)
{
	dwCurPlayerActor = pPed;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);
	uint8_t old = CWorld::PlayerInFocus;
	CWorld::PlayerInFocus = byteCurPlayer;

	bool result =  ((bool(*)(uintptr_t*, uintptr_t))(g_libGTASA + 0x00539F9C + 1))(thiz, pPed);

	CWorld::PlayerInFocus = old;
	return result;
}

bool bWeaponClicked;

//switch weapons
extern "C" {
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onWeaponChanged(JNIEnv *pEnv, jobject thiz) {
		//pGame->FindPlayerPed()->SwitchWeapon();
		if(!pGame->FindPlayerPed()) return;

		if(!bWeaponClicked) {
			bWeaponClicked = true;
		}
		else {
			bWeaponClicked = false;
		}
	}
}
extern CGUI *pGUI;

uint32_t (*CPad__CycleWeaponRightJustDown)(uintptr_t thiz);
uint32_t CPad__CycleWeaponRightJustDown_hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0)) return 0;

	if(!bWeaponClicked) {
		return 0;
	}
	else {
		bWeaponClicked = false;
		return 1;
	}
	return CPad__CycleWeaponRightJustDown(thiz);
}


uint32_t (*CCamera_IsTargetingActive)(uintptr_t thiz);
uint32_t CCamera_IsTargetingActive_hook(uintptr_t thiz)
{
	return 1;

}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	// fix aiming problems
	//memcpy((void*)(g_libGTASA + 0x008C3E20), , )

	// CPed::ProcessControl
	CHook::InlineHook(g_libGTASA, 0x004C47E8, &CPed__ProcessControl_hook, &CPed__ProcessControl);

//	// all vehicles ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066D6A4, &AllVehicles__ProcessControl_hook); // CAutomobile::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066DA4C, &AllVehicles__ProcessControl_hook); // CBoat::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066D81C, &AllVehicles__ProcessControl_hook); // CBike::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066DDB0, &AllVehicles__ProcessControl_hook); // CPlane::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066DB60, &AllVehicles__ProcessControl_hook); // CHeli::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066D934, &AllVehicles__ProcessControl_hook); // CBmx::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066DC88, &AllVehicles__ProcessControl_hook); // CMonsterTruck::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066DED8, &AllVehicles__ProcessControl_hook); // CQuadBike::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066E128, &AllVehicles__ProcessControl_hook); // CTrain::ProcessControl
//	CHook::MethodHook(g_libGTASA, 0x0066968C, &TaskUseGun);
	CHook::MethodHook(g_libGTASA, 0x0066CF4C, &CPad__TaskProcess);
//
//	// lr/ud (onfoot)
	CHook::InlineHook(g_libGTASA, 0x003FA218, &CPad__GetPedWalkLeftRight_hook, &CPad__GetPedWalkLeftRight);
	CHook::InlineHook(g_libGTASA, 0x003FA298, &CPad__GetPedWalkUpDown_hook, &CPad__GetPedWalkUpDown);
//
//	// look
////	SetUpHook(g_libGTASA+0x0039D194, (uintptr_t)CPad__GetLookLeft_hook, (uintptr_t*)&CPad__GetLookLeft);
////	SetUpHook(g_libGTASA+0x0039D26C, (uintptr_t)CPad__GetLookRight_hook, (uintptr_t*)&CPad__GetLookRight);
//
//	CHook::InlineHook(g_libGTASA, 0x003FA4A0, &CPad__GetTurretLeft_hook, &CPad__GetTurretLeft);
//	CHook::InlineHook(g_libGTASA, 0x003FA4B6, &CPad__GetTurretRight_hook, &CPad__GetTurretRight);
//
////	SetUpHook(g_libGTASA+0x0039E64C, (uintptr_t)CPad__ShiftTargetLeftJustDown_hook, (uintptr_t*)&CPad__ShiftTargetLeftJustDown);
////	SetUpHook(g_libGTASA+0x0039E6CC, (uintptr_t)CPad__ShiftTargetRightJustDown_hook, (uintptr_t*)&CPad__ShiftTargetRightJustDown);
//	//SetUpHook(g_libGTASA+0x39D110, (uintptr_t)CPad__GetPedWalkUpDown_hook, (uintptr_t*)&CPad__GetPedWalkUpDown);
//	//SetUpHook(g_libGTASA + 0x)
//
//	// sprint/jump stuff
//	CHook::InlineHook(g_libGTASA, 0x003FBE64, &CPad__SprintJustDown_hook, &CPad__SprintJustDown);// ускоренныйбег
//
//
//	CHook::InlineHook(g_libGTASA, 0x3FBDB0, &CPad__GetSprint_hook, &CPad__GetSprint);
//	CHook::InlineHook(g_libGTASA, 0x003FBCAC, &CPad__JumpJustDown_hook, &CPad__JumpJustDown);
//	CHook::InlineHook(g_libGTASA, 0x003FBC58, &CPad__GetJump_hook, &CPad__GetJump);
//	CHook::InlineHook(g_libGTASA, 0x003FBB0C, &CPad__GetAutoClimb_hook, &CPad__GetAutoClimb);
//	CHook::InlineHook(g_libGTASA, 0x003FBBB8, &CPad__GetAbortClimb_hook, &CPad__GetAbortClimb);
//	//SetUpHook(g_libGTASA+0x0037440C, (uintptr_t)CCamera_IsTargetingActive_hook, (uintptr_t*)&CCamera_IsTargetingActive);
//
//	// swimm
//	CHook::InlineHook(g_libGTASA, 0x003FBD10, &CPad__DiveJustDown_hook, (uintptr_t*)&CPad__DiveJustDown);
//	CHook::InlineHook(g_libGTASA, 0x003FBD58, &CPad__SwimJumpJustDown_hook, (uintptr_t*)&CPad__SwimJumpJustDown);
//
//	CHook::InlineHook(g_libGTASA, 0x003FB00C, &CPad__MeleeAttackJustDown_hook, (uintptr_t*)&CPad__MeleeAttackJustDown);
//
//	CHook::InlineHook(g_libGTASA, 0x003FABA8, &CPad__GetWeapon_hook, &CPad__GetWeapon);
//	CHook::InlineHook(g_libGTASA, 0x003FB4EC, &CPad__GetEnterTargeting_hook, &CPad__GetEnterTargeting);
//	CHook::InlineHook(g_libGTASA, 0x003FB704, &GetTarget_hook, &GetTarget);
//	CHook::InlineHook(g_libGTASA, 0x00537808, &ProcessPlayerWeapon_hook, &ProcessPlayerWeapon);
//
//	//SetUpHook(g_libGTASA+0x39E7B0, (uintptr_t)CPad__DuckJustDown_hook, (uintptr_t*)&CPad__DuckJustDown);
//	CHook::InlineHook(g_libGTASA, 0x003FBA9C, &CPad__DuckJustDown_hook, &CPad__DuckJustDown);
//
//
//	// steering lr/ud (incar)
	CHook::InlineHook(g_libGTASA, 0x003F9B54, &CPad__GetSteeringLeftRight_hook, &CPad__GetSteeringLeftRight);
	CHook::InlineHook(g_libGTASA, 0x003F9D24, &CPad__GetSteeringUpDown_hook, &CPad__GetSteeringUpDown);
//
	CHook::InlineHook(g_libGTASA, 0x003FB350, &CPad__GetAccelerate_hook, &CPad__GetAccelerate);
	CHook::InlineHook(g_libGTASA, 0x003FA9AC, &CPad__GetBrake_hook, &CPad__GetBrake);
//	CHook::InlineHook(g_libGTASA, 0x003FA7E0, &CPad__GetHandBrake_hook, &CPad__GetHandBrake);
//	CHook::InlineHook(g_libGTASA, 0x003FA618, &CPad__GetHorn_hook, &CPad__GetHorn);
//	CHook::InlineHook(g_libGTASA, 0x00584CEC, &CVehicle__UsesSiren_hook, &CVehicle__UsesSiren);
//
//	CHook::InlineHook(g_libGTASA, 0x003FB2AC, &CPad__CycleWeaponRightJustDown_hook, &CPad__CycleWeaponRightJustDown);

	//SetUpHook(g_libGTASA+0x00351C40, (uintptr_t)IsVehicleRadioActive_hook, (uintptr_t*)&IsVehicleRadioActive);

}
#include <sstream>
#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"
#include "../CSettings.h"
#include "../util/CJavaWrapper.h"
#include "java_systems/CHUD.h"
#include "java_systems/CEditobject.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

int iNetModeNormalOnfootSendRate	= NETMODE_ONFOOT_SENDRATE;
int iNetModeNormalInCarSendRate		= NETMODE_INCAR_SENDRATE;
int iNetModeFiringSendRate			= NETMODE_FIRING_SENDRATE;
int iNetModeSendMultiplier 			= NETMODE_SEND_MULTIPLIER;


void EditAttachedObject(RPCParameters *rpcParams) {
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	uint32_t index;

	bsData.Read(index);

	CEditobject::StartEditAttachedObject(index);

	Log("RPC: EditAttachedObject %d", index);
}
void InitGame(RPCParameters *rpcParams)
{
	Log("RPC: InitGame");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID MyPlayerID;
	bool bLanMode, bStuntBonus;

	RakNet::BitStream bsInitGame(Data,(iBitLength/8)+1,false);

	bsInitGame.ReadCompressed(pNetGame->m_bZoneNames);							// unknown
	bsInitGame.ReadCompressed(pNetGame->m_bUseCJWalk);							// native UsePlayerPedAnims(); +
	bsInitGame.ReadCompressed(pNetGame->m_bAllowWeapons);						// native AllowInteriorWeapons(allow); +
	bsInitGame.ReadCompressed(pNetGame->m_bLimitGlobalChatRadius);				// native LimitGlobalChatRadius(Float:chat_radius); +
	bsInitGame.Read(pNetGame->m_fGlobalChatRadius);								// +
	bsInitGame.ReadCompressed(bStuntBonus);										// native EnableStuntBonusForAll(enable); +
	bsInitGame.Read(pNetGame->m_fNameTagDrawDistance);							// native SetNameTagDrawDistance(Float:distance); +
	bsInitGame.ReadCompressed(pNetGame->m_bDisableEnterExits);					// native DisableInteriorEnterExits(); +
	bsInitGame.ReadCompressed(pNetGame->m_bNameTagLOS);							// native DisableNameTagLOS(); +
	bsInitGame.ReadCompressed(pNetGame->m_bManualVehicleEngineAndLight);		// native ManualVehicleEngineAndLights(); +
	bsInitGame.Read(pNetGame->m_iSpawnsAvailable);								// +
	bsInitGame.Read(MyPlayerID);												// 
	bsInitGame.ReadCompressed(pNetGame->m_bShowPlayerTags);						// native ShowNameTags(show); +
	bsInitGame.Read(pNetGame->m_iShowPlayerMarkers);							// native ShowPlayerMarkers(mode); +
	bsInitGame.Read(pNetGame->m_byteWorldTime);									// native SetWorldTime(hour); +
	bsInitGame.Read(pNetGame->m_byteWeather);									// native SetWeather(weatherid); +
	bsInitGame.Read(pNetGame->m_fGravity);										// native SetGravity(Float:gravity); +
	bsInitGame.ReadCompressed(bLanMode);										// 
	bsInitGame.Read(pNetGame->m_iDeathDropMoney);								// native SetDeathDropAmount(amount); +
	bsInitGame.ReadCompressed(pNetGame->m_bInstagib);							// always 0

	bsInitGame.Read(iNetModeNormalOnfootSendRate);
	bsInitGame.Read(iNetModeNormalInCarSendRate);
	bsInitGame.Read(iNetModeFiringSendRate);
	bsInitGame.Read(iNetModeSendMultiplier);

	bsInitGame.Read(pNetGame->m_iLagCompensation);								// lagcomp +

	uint8_t byteStrLen;
	bsInitGame.Read(byteStrLen);
	if(byteStrLen)																// SetGameModeText(); +
	{
		memset(pNetGame->m_szHostName, 0, sizeof(pNetGame->m_szHostName));
		bsInitGame.Read(pNetGame->m_szHostName, byteStrLen);
	}
	pNetGame->m_szHostName[byteStrLen] = '\0';

	uint8_t byteVehicleModels[212];
	bsInitGame.Read((char*)&byteVehicleModels[0], 212);							// don't use?
	bsInitGame.Read(pNetGame->m_iVehicleFriendlyFire);							// native EnableVehicleFriendlyFire(); +

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = nullptr;
	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

	pGame->SetGravity(pNetGame->m_fGravity);

	if(pNetGame->m_bDisableEnterExits)
		pGame->DisableInteriorEnterExits();

	pNetGame->SetGameState(GAMESTATE_CONNECTED);
	if(pLocalPlayer) pLocalPlayer->HandleClassSelection();

	pGame->SetWorldWeather(pNetGame->m_byteWeather);
	pGame->ToggleCJWalk(pNetGame->m_bUseCJWalk);
	pNetGame->GetPlayerPool()->SetLocalPlayerID(MyPlayerID);
}

void ServerJoin(RPCParameters *rpcParams)
{
	//Log("RPC: ServerJoin");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szPlayerName[MAX_PLAYER_NAME+1];
	PLAYERID playerId;
	unsigned char byteNameLen = 0;
	uint8_t bIsNPC = 0;
	int pading;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(pading);
	bsData.Read(bIsNPC);
	bsData.Read(byteNameLen);
	bsData.Read(szPlayerName, byteNameLen);
	szPlayerName[byteNameLen] = '\0';

	pPlayerPool->New(playerId, szPlayerName, bIsNPC);
}

void ServerQuit(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteReason;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(byteReason);


	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	pPlayerPool->Delete(playerId, byteReason);
}

void ClientMessage(RPCParameters *rpcParams)
{

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint32_t dwStrLen;
	uint32_t dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);
	char* szMsg = (char*)malloc(dwStrLen+1);
	bsData.Read(szMsg, dwStrLen);
	szMsg[dwStrLen] = 0;

	std::string msg = szMsg;

	char buf[dwStrLen+55];
	snprintf(buf, sizeof(buf), "{%x}%s", (dwColor>>8)&0xFFFFFF, szMsg);

	CHUD::AddChatMessage(buf);

	free(szMsg);
}

void Chat(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	uint8_t byteTextLen;

	if(pNetGame->GetGameState() != GAMESTATE_CONNECTED)	return;

	unsigned char szText[256];
	memset(szText, 0, 256);

	bsData.Read(playerId);
	bsData.Read(byteTextLen);
	bsData.Read((char*)szText,byteTextLen);

	szText[byteTextLen] = '\0';

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (playerId == pPlayerPool->GetLocalPlayerID())
	{
		CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
		if (pLocalPlayer) 
		{
			CChatWindow::AddChatMessage(pPlayerPool->GetLocalPlayerName(),
			pLocalPlayer->GetPlayerColor(), (char*)szText);
		}
	} 
	else 
	{
		CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
		if(pRemotePlayer)
			pRemotePlayer->Say(szText);
	}
}

void RequestClass(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteRequestOutcome = 0;
	PLAYER_SPAWN_INFO SpawnInfo;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = 0;

	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

	bsData.Read(byteRequestOutcome);
	bsData.Read((char*)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	if(pLocalPlayer)
	{
		if(byteRequestOutcome)
		{
			pLocalPlayer->SetSpawnInfo(&SpawnInfo);
			pLocalPlayer->HandleClassSelectionOutcome();
		}
	}
}

void Weather(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteWeather;
	bsData.Read(byteWeather);
	pNetGame->m_byteWeather = byteWeather;
	pGame->SetWorldWeather(byteWeather);
}

void RequestSpawn(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint32_t byteRequestOutcome = 0;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(byteRequestOutcome);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = 0;
	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

	if(pLocalPlayer)
	{
		if(byteRequestOutcome == 2 || (byteRequestOutcome && pLocalPlayer->m_bWaitingForSpawnRequestReply))
			 pLocalPlayer->Spawn();
		else
			pLocalPlayer->m_bWaitingForSpawnRequestReply = false;
	}
}

void WorldTime(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteWorldTime;
	bsData.Read(byteWorldTime);
	pNetGame->m_byteWorldTime = byteWorldTime;
}

void SetTimeEx(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteHour;
	uint8_t byteMinute;
	bsData.Read(byteHour);
	bsData.Read(byteMinute);

	pGame->SetWorldTime(byteHour, byteMinute);
	pNetGame->m_byteWorldTime = byteHour;
	pNetGame->m_byteWorldMinute = byteMinute;
}

void WorldPlayerAdd(RPCParameters *rpcParams)
{

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	PLAYERID playerId;
	uint8_t byteFightingStyle=4;
	uint8_t byteTeam=0;
	unsigned int iSkin=0;
	CVector vecPos;
	float fRotation=0;
	uint32_t dwColor=0;
	bool bVisible;

	bsData.Read(playerId);
	bsData.Read(byteTeam);
	bsData.Read(iSkin);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fRotation);
	bsData.Read(dwColor);
	bsData.Read(byteFightingStyle);
	bsData.Read(bVisible);

	if(pPlayerPool)
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(playerId);
		if(pRemotePlayer) pRemotePlayer->Spawn(byteTeam, iSkin, &vecPos, fRotation, dwColor, byteFightingStyle, bVisible);
	}

}

void WorldPlayerRemove(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	bsData.Read(playerId);

	if(pPlayerPool)
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer)
		{
			pRemotePlayer->Remove();
		}
	}
}

void SetCheckpoint(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	float fX, fY, fZ, fSize;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fSize);

	CVector pos, Extent;

	pos.x = fX;
	pos.y = fY;
	pos.z = fZ;
	Extent.x = fSize;
	Extent.y = fSize;
	Extent.z = fSize;

	pGame->SetCheckpointInformation(&pos, &Extent);
	pGame->CreateCheckPoint();
}

void DisableCheckpoint(RPCParameters *rpcParams)
{
	pGame->DisableCheckpoint();
}

void SetRaceCheckpoint(RPCParameters *rpcParams)
{
	unsigned char *Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	float fX, fY, fZ;
	uint8_t byteType;
	CVector pos, next;

	bsData.Read(byteType);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	pos.x = fX;
	pos.y = fY;
	pos.z = fZ;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	next.x = fX;
	next.y = fY;
	next.z = fZ;

	bsData.Read(fX);

	pGame->SetRaceCheckpointInformation(byteType, &pos, &next, fX);
}

void DisableRaceCheckpoint(RPCParameters *rpcParams)
{
	if(pGame->m_bRaceCheckpointsEnabled)
	{
		pGame->DisableRaceCheckpoint();
		CHUD::toggleGps(false);
	}
}
void WorldVehicleAdd(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) return;

	NEW_VEHICLE NewVehicle;
	bsData.Read((char *)&NewVehicle,sizeof(NEW_VEHICLE));

	//if(NewVehicle.iVehicleType < 400 || NewVehicle.iVehicleType > 611) return;
	if (!pVehiclePool->New(&NewVehicle))
		return;
	int iVehicle = pVehiclePool->FindGtaIDFromID(NewVehicle.VehicleID);
	if (iVehicle)
	{
		for (int i = 0; i < 14; i++)
		{
			int data = (int)NewVehicle.byteModSlots[i];
			uint32_t v = 0;

			if (data == 0)
				continue;

			data += 999;
			pGame->RequestModel(data);
			pGame->LoadRequestedModels();
			ScriptCommand(&request_car_component, data);
			int iWait = 10;
			while (!ScriptCommand(&is_component_available, data) && iWait)
			{
				usleep(5);
				iWait--;
			}
			if (!iWait)
			{
				continue;
			}
			ScriptCommand(&add_car_component, iVehicle, data, &v);
		}

//		if (NewVehicle.bytePaintjob)
//		{
//			{
//				ScriptCommand(&change_car_skin, iVehicle, (NewVehicle.bytePaintjob - 1));
//				CVehicleGta* pVeh = GamePool_Vehicle_GetAt(iVehicle);
//				if (pVeh)
//				{
//					uintptr_t this_vtable = pVeh->entity.vtable;
//					this_vtable -= g_libGTASA;
//
//					if (this_vtable == 0x5CC9F0)
//					{
//						ScriptCommand(&change_car_skin, iVehicle, (NewVehicle.bytePaintjob - 1));
//					}
//				}
//			}
//		}
		if (NewVehicle.cColor1 != -1 || NewVehicle.cColor2 != -1)
		{
			pVehiclePool->GetAt(NewVehicle.VehicleID)->SetColor(NewVehicle.cColor1, NewVehicle.cColor2);
		}
	}
}

void WorldVehicleRemove(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	if(!pVehiclePool) return;

	VEHICLEID VehicleID;
	VEHICLEID MyVehicleID;

	bsData.Read(VehicleID);

	pVehiclePool->Delete(VehicleID);
}

void EnterVehicle(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	VEHICLEID VehicleID=0;
	uint8_t bytePassenger=0;

	bsData.Read(playerId);
	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool)
	{
		CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
		if(pPlayer) {
			Log("RPC_EnterVehicle VehicleID = %d, playerId = %d,  bytePassenger = %d",
				VehicleID, playerId, bytePassenger);
			pPlayer->EnterVehicle(VehicleID, (bool) bytePassenger);
		}
	}	
}

void ExitVehicle(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	PLAYERID playerId;
	VEHICLEID VehicleID = 0;

	bsData.Read(playerId);
	bsData.Read(VehicleID);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool)
	{
		CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
		if(pPlayer)
			pPlayer->ExitVehicle();
	}	
}

void GameModeRestart(RPCParameters *rpcParams)
{
	// CChatWindow::AddInfoMessage("The server is restarting..");
	CChatWindow::AddInfoMessage("{ff0000}Ñåðâåð ïåðåçàãðóæàåòñÿ...");
	pNetGame->ShutDownForGameRestart();
}

#define REJECT_REASON_BAD_VERSION   1
#define REJECT_REASON_BAD_NICKNAME  2
#define REJECT_REASON_BAD_MOD		3
#define REJECT_REASON_BAD_PLAYERID	4
void ConnectionRejected(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteRejectReason;

	bsData.Read(byteRejectReason);

	if(byteRejectReason == REJECT_REASON_BAD_VERSION)
		CChatWindow::AddInfoMessage("ÎÒÊËÞ×ÅÍÈÅ ÎÒÊËÞ×ÅÍÎ. ÑÅÐÂÅÐ ÍÅ ÐÀÇÐÅØÈË ÑÎÅÄÈÍÅÍÈÅ.");
	else if(byteRejectReason == REJECT_REASON_BAD_NICKNAME)
	{
		CChatWindow::AddInfoMessage("ÑÎÅÄÈÍÅÍÈÅ ÎÒÊËÎÍÅÍÎ. ÍÅÏÐÀÂÈËÜÍÛÉ ÍÈÊ!");
		CChatWindow::AddInfoMessage("Ïîæàëóéñòà, âûáåðèòå äðóãîé íèê èç 5-20 ñèìâîëîâ");
		CChatWindow::AddInfoMessage("ñîäåðæàùèé òîëüêî A-Z a-z 0-9 [] èëè _");
		CChatWindow::AddInfoMessage("Èñïîëüçóéòå /quit äëÿ âûõîäà èëè íàæìèòå ESC è âûáåðèòå Âûéòè èç èãðû");
	}
	else if(byteRejectReason == REJECT_REASON_BAD_MOD)
	{
		CChatWindow::AddInfoMessage("ÑÎÅÄÈÍÅÍÈÅ ÎÒÊËÎÍÅÍÎ");
		CChatWindow::AddInfoMessage("ÂÛ ÈÑÏÎËÜÇÓÅÒÅ ÍÅÏÐÀÂÈËÜÍÛÉ ÌÎÄ!");
	}
	else if(byteRejectReason == REJECT_REASON_BAD_PLAYERID)
	{
		CChatWindow::AddInfoMessage("Ñîåäèíåíèå áûëî çàêðûòî ñåðâåðîì.");
		CChatWindow::AddInfoMessage("Íå óäàëîñü âûäåëèòü ñëîò äëÿ èãðîêà. Ïîâòîðèòå ïîïûòêó.");
	}

	pNetGame->GetRakClient()->Disconnect(500);
}

void Pickup(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	PICKUP Pickup;
	int iIndex;

	bsData.Read(iIndex);
	bsData.Read((char*)&Pickup, sizeof (PICKUP));

	CPickupPool *pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) pPickupPool->New(&Pickup, iIndex);
}

void DestroyPickup(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	int iIndex;
	bsData.Read(iIndex);

	CPickupPool *pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) pPickupPool->Destroy(iIndex);
}

void Create3DTextLabel(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t LabelID;
	uint32_t color;
	CVector pos;
	float dist;
	uint8_t testLOS;
	PLAYERID PlayerID;
	VEHICLEID VehicleID;
	char szBuff[4096+1];

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	bsData.Read(LabelID);
	bsData.Read(color);
	bsData.Read(pos.x);
	bsData.Read(pos.y);
	bsData.Read(pos.z);
	bsData.Read(dist);
	bsData.Read(testLOS);
	bsData.Read(PlayerID);
	bsData.Read(VehicleID);

	stringCompressor->DecodeString(szBuff, 4096, &bsData);

	CText3DLabelsPool *pLabelsPool = pNetGame->GetLabelPool();

	if(pLabelsPool)
	{
		pLabelsPool->CreateTextLabel((int)LabelID, szBuff, color, 
			pos.x, pos.y, pos.z, dist, testLOS, PlayerID, VehicleID);
	}
}

void Delete3DTextLabel(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t LabelID;
	bsData.Read(LabelID);

	CText3DLabelsPool *pLabelsPool = pNetGame->GetLabelPool();
	if(pLabelsPool)
	{
		pLabelsPool->Delete((int)LabelID);
	}
}

void Update3DTextLabel(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t LabelID;
	bsData.Read(LabelID);

	CText3DLabelsPool *pLabelsPool = pNetGame->GetLabelPool();
	if(pLabelsPool)
	{
		//pLabelsPool->Delete(LabelID);
	}
}
extern bool bFirstSpawn;
#define EVENT_TYPE_PAINTJOB			1
#define EVENT_TYPE_CARCOMPONENT		2
#define EVENT_TYPE_CARCOLOR			3
#define EVENT_ENTEREXIT_MODSHOP		4
void ProcessIncommingEvent(BYTE bytePlayerID, int iEventType, uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3)
{
	uint32_t v;
	int iVehicleID;
	int iPaintJob;
	int iComponent;
	int iWait;
	CVehicle* pVehicle;
	CRemotePlayer* pRemote;

	if (!pNetGame) return;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	switch (iEventType) 
	{

	case EVENT_TYPE_PAINTJOB:
		iVehicleID = pVehiclePool->FindGtaIDFromID(dwParam1);
		iPaintJob = (int)dwParam2;
		if (iVehicleID) ScriptCommand(&change_car_skin, iVehicleID, dwParam2);
		break;

	case EVENT_TYPE_CARCOMPONENT:
		Log("RPC: EVENT_TYPE_CARCOMPONENT");
		iVehicleID = pVehiclePool->FindGtaIDFromID(dwParam1);
		iComponent = (int)dwParam2;
		pGame->RequestModel(iComponent);
		pGame->LoadRequestedModels();
		ScriptCommand(&request_car_component, iComponent);

		iWait = 10;
		while (!ScriptCommand(&is_component_available, iComponent) && iWait) 
		{
			usleep(5);
			iWait--;
		}
		if (!iWait) 
		{
			CChatWindow::AddDebugMessage("Timeout on car component.");
			break;
		}
		ScriptCommand(&add_car_component, iVehicleID, iComponent, &v);
		CChatWindow::AddDebugMessage("Added car component: %d",iComponent);
		break;

	case EVENT_TYPE_CARCOLOR:
		pVehicle = pVehiclePool->GetAt((VEHICLEID)dwParam1);
		if (pVehicle)
		{
			pVehicle->SetColor((int)dwParam2, (int)dwParam3);
		}
		break;

	/*case EVENT_ENTEREXIT_MODSHOP:
		if (bytePlayerID > MAX_PLAYERS) return;
		pVehicle = pVehiclePool->GetAt((BYTE)dwParam1);
		pRemote = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
		if (pRemote && pVehicle) 
		{
			//pVehicle->SetLockedState((int)dwParam2); // Results in a crash at 0x48CFC9 under certain conditions.
			pRemote->m_iIsInAModShop = (int)dwParam2;
		}
		break;*/
	}
}

void ScmEvent(RPCParameters* rpcParams)
{
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID bytePlayerID;
	int iEvent;
	uint32_t dwParam1, dwParam2, dwParam3;

	bsData.Read(bytePlayerID);
	bsData.Read(iEvent);
	bsData.Read(dwParam1);
	bsData.Read(dwParam2);
	bsData.Read(dwParam3);
	ProcessIncommingEvent(bytePlayerID, iEvent, dwParam1, dwParam2, dwParam3);
}

void RemoveBuildingByPtr(uintptr_t pBuild)
{
	CVector* vecObjectPos = (CVector*)(pBuild + 4);
	vecObjectPos->z -= 2000.0f;
	*(uint8_t*)(pBuild + 47) = 1;
	if (*(uintptr_t*)(pBuild + 20))
	{
		MATRIX4X4* matt = (MATRIX4X4*) * (uintptr_t*)(pBuild + 20);
		matt->pos.z -= 2000.0f;
		//*(uint32_t*)((uintptr_t)matt + 12) &= 0xFFFDFFFC;
	}
}

void* GetObjectPool()
{
	return (void*) * (uintptr_t*)(g_libGTASA + 0x008B93C8);
}

void* GetBuildingPool()
{
	return (void*) * (uintptr_t*)(g_libGTASA + 0x008B93CC);
}

void* GetDummyPool()
{
	return (void*) * (uintptr_t*)(g_libGTASA + 0x008B93C4);
}


void RemoveOccluders(float X, float Y, float Z, float fRad)
{
	uintptr_t* numOccluders = (uintptr_t*)(g_libGTASA + 0x009A0FEC);
	if (*numOccluders > 0)
	{
		char* v5 = (char*)(g_libGTASA + 0x009A0FF4);
		for (int i = 0; i < *numOccluders; i++)
		{
			double v6 = (double) * (int16_t*)v5 * 0.25;
			double v7 = (double) * ((int16_t*)v5 - 1) * 0.25;
			double v8 = (double) * ((int16_t*)v5 - 2) * 0.25;
			CVector f = { (float)v8, (float)v7, (float)v6 };
			CVector s = { X, Y, Z };
			if (GetDistanceBetween3DPoints(&f, &s) < fRad)
			{
				*((int16_t*)v5 - 2) = 0;
				*((int16_t*)v5 - 1) = 0;
				*(int16_t*)v5 = 0;
				*((int16_t*)v5 + 1) = 0;
				*((int16_t*)v5 + 2) = 0;
				*((int16_t*)v5 + 3) = 0;
			}
			v5 += 18;
		}
	}
}

void ResetPoolsMatrix()
{
	uintptr_t pBuild = *(uintptr_t*)GetBuildingPool();

	for (int i = 0; i < 14000; i++)
	{
		*(uintptr_t*)(pBuild + 20) = 0;
		pBuild += 0x38;
	}
	pBuild = *(uintptr_t*)GetDummyPool();
	for (int i = 0; i < 3500; i++)
	{
		*(uintptr_t*)(pBuild + 20) = 0;
		pBuild += 0x38;
	}

	pBuild = *(uintptr_t*)GetObjectPool();
	for (int i = 0; i < 350; i++)
	{
		*(uintptr_t*)(pBuild + 20) = 0;
		pBuild += 0x1A0;
	}
}

void ProcessRemoveBuilding(int uModelID, CVector pos, float fRad)
{
	uintptr_t pBuild = *(uintptr_t*)GetBuildingPool();
	uintptr_t pState = *(uintptr_t*)((uintptr_t)GetBuildingPool() + 4);
	// pBuild + 34 = nModelIndex
	// 0x38 - sizeof(CBuilding)
	// pBuild + 4 - CSimpleTransform
	// pBuild + 20 - MATRIX4X4*

	RemoveOccluders(pos.x, pos.y, pos.z, 500.0);

	for (int i = 0; i < 14000; i++)
	{
		uintptr_t vtable = *(uintptr_t*)(pBuild);
		vtable -= g_libGTASA;
		if (vtable == 0x5C7358 || (*(uint8_t*)pState & 0x80))
		{
			pBuild += 0x38;
			pState++;
			continue;
		}
		if (*(uint16_t*)(pBuild + 34) == uModelID || uModelID == -1)
		{
			if (*(uintptr_t*)(pBuild + 20) && (*(uintptr_t*)(pBuild + 20) != 0xffffff)
				&& (*(uintptr_t*)(pBuild + 20) != 0xffffffff))
			{
				MATRIX4X4* matt = (MATRIX4X4*) * (uintptr_t*)(pBuild + 20);
				if (GetDistanceBetween3DPoints(&pos, &matt->pos) <= fRad)
				{
					RemoveBuildingByPtr(pBuild);
				}
			}
			else
			{
				CVector* vecObjectPos = (CVector*)(pBuild + 4);
				if (GetDistanceBetween3DPoints(&pos, vecObjectPos) <= fRad)
				{
					RemoveBuildingByPtr(pBuild);
				}
			}
		}
		pBuild += 0x38;
		pState++;
	}
	pBuild = *(uintptr_t*)GetDummyPool();
	pState = *(uintptr_t*)((uintptr_t)GetDummyPool() + 4);
	for (int i = 0; i < 3500; i++)
	{
		uintptr_t vtable = *(uintptr_t*)(pBuild);
		vtable -= g_libGTASA;
		if (vtable == 0x5C7358 || (*(uint8_t*)pState & 0x80))
		{
			pBuild += 0x38;
			pState++;
			continue;
		}
		if (*(uint16_t*)(pBuild + 34) == uModelID || uModelID == -1)
		{
			if (*(uintptr_t*)(pBuild + 20) && (*(uintptr_t*)(pBuild + 20) != 0xffffff)
				&& (*(uintptr_t*)(pBuild + 20) != 0xffffffff))
			{
				MATRIX4X4* matt = (MATRIX4X4*) * (uintptr_t*)(pBuild + 20);
				if (GetDistanceBetween3DPoints(&pos, &matt->pos) <= fRad)
				{
					RemoveBuildingByPtr(pBuild);
				}
			}
			else
			{
				CVector* vecObjectPos = (CVector*)(pBuild + 4);
				if (GetDistanceBetween3DPoints(&pos, vecObjectPos) <= fRad)
				{
					RemoveBuildingByPtr(pBuild);
				}
			}
		}
		pBuild += 0x38;
		pState++;
	}

	pBuild = *(uintptr_t*)GetObjectPool();
	pState = *(uintptr_t*)((uintptr_t)GetObjectPool() + 4);
	for (int i = 0; i < 350; i++)
	{
		uintptr_t vtable = *(uintptr_t*)(pBuild);
		vtable -= g_libGTASA;
		if (vtable == 0x5C7358 || (*(uint8_t*)pState & 0x80))
		{
			pBuild += 0x1A0;
			pState++;
			continue;
		}
		if (*(uint16_t*)(pBuild + 34) == uModelID || uModelID == -1)
		{
			if (*(uintptr_t*)(pBuild + 20) && (*(uintptr_t*)(pBuild + 20) != 0xffffff)
				&& (*(uintptr_t*)(pBuild + 20) != 0xffffffff))
			{
				MATRIX4X4* matt = (MATRIX4X4*) * (uintptr_t*)(pBuild + 20);
				if (GetDistanceBetween3DPoints(&pos, &matt->pos) <= fRad)
				{
					RemoveBuildingByPtr(pBuild);
				}
			}
			else
			{
				CVector* vecObjectPos = (CVector*)(pBuild + 4);
				if (GetDistanceBetween3DPoints(&pos, vecObjectPos) <= fRad)
				{
					RemoveBuildingByPtr(pBuild);
				}
			}
		}
		pBuild += 0x1A0;
		pState++;
	}
}

extern int RemoveModelIDs[1200];
extern CVector RemovePos[1200];
extern float RemoveRad[1200];
extern int iTotalRemovedObjects;

void RemoveBuilding(RPCParameters* rpcParams)
{
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	int uModelID;
	CVector pos;
	float fRad;

	bsData.Read(uModelID);
	bsData.Read((char*)& pos, sizeof(CVector));
	bsData.Read(fRad);
	RemoveModelIDs[iTotalRemovedObjects] = uModelID;
	RemovePos[iTotalRemovedObjects] = pos;
	RemoveRad[iTotalRemovedObjects] = fRad;
	iTotalRemovedObjects++;
	ProcessRemoveBuilding(uModelID, pos, fRad);
}
#include "..//gui/gui.h"
#include "../playertags.h"
#include "java_systems/CTab.h"
#include "java_systems/CDialog.h"

extern CPlayerTags* pPlayerTags;

void SetPlayerChatBubble(RPCParameters* rpcParams)
{
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	//PLAYERID playerId;
	uint16_t playerId;
	//if (playerId < 0 || playerId > MAX_PLAYERS) return;
	uint32_t color;
	float drawDistance;
	uint32_t expireTime;
	uint8_t texLength;

	bsData.Read(playerId);
	bsData.Read(color);
	bsData.Read(drawDistance);
	bsData.Read(expireTime);
	bsData.Read(texLength);
	if (texLength >= 255) return;
	char pText[256];
	bsData.Read(pText, texLength);
	pText[texLength] = '\0';
	if (pPlayerTags)
	{
		pPlayerTags->AddChatBubble(playerId, pText, color, drawDistance, expireTime);
	}
}

void WorldPlayerDeath(RPCParameters* rpcParams)
{
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	bsData.Read(playerId);
	if (pNetGame->GetPlayerPool())
	{
		CRemotePlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt(playerId);
		if (pPlayer)
		{
			pPlayer->HandleDeath();
		}
	}
}

void DamageVehicle(RPCParameters* rpcParams)
{
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	VEHICLEID vehId;
	bsData.Read(vehId);

	if (pNetGame->GetVehiclePool()->GetSlotState(vehId))
	{
		uint32_t dwPanelStatus, dwDoorStatus;
		uint8_t byteLightStatus, byteTireStatus;

		bsData.Read(dwPanelStatus);
		bsData.Read(dwDoorStatus);
		bsData.Read(byteLightStatus);
		bsData.Read(byteTireStatus);

		CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
		if (pVehicle)
			pVehicle->UpdateDamageStatus(dwPanelStatus, dwDoorStatus, byteLightStatus, byteTireStatus);
	}
}

void WorldActorAdd(RPCParameters* rpcParams)
{
	

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	uint32_t iSkinId;
	CVector vecPos;
	float fRotation;
	float fHealth;
	bool bInvulnerable;

	bsData.Read(actorId);
	bsData.Read(iSkinId);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fRotation);
	bsData.Read(fHealth);
	bsData.Read(bInvulnerable);

#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("Add actor %d %d %f %f", actorId, fRotation, fHealth);
#endif
	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (pActorPool) 
	{
		pActorPool->Spawn(actorId, iSkinId, vecPos, fRotation, fHealth, bInvulnerable);
	}
}

void WorldActorRemove(RPCParameters* rpcParams)
{
	

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;

	bsData.Read(actorId);
	
#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("DESTROY actor %d", actorId);
#endif
	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (pActorPool) 
	{
		pActorPool->Delete(actorId);
	}
}

void SetActorHealth(RPCParameters* rpcParams)
{
	

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	float fHealth;
	bsData.Read(actorId);
	bsData.Read(fHealth);
#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("Set actor h[p %d %f", actorId, fHealth);
#endif
	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (pActorPool)
	{
		if (pActorPool->GetAt(actorId))
		{
			pActorPool->GetAt(actorId)->SetHealth(fHealth);
		}
	}
}

void SetActorPos(RPCParameters* rpcParams)
{
	

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	CVector pos;
	bsData.Read(actorId);
	bsData.Read((char*)& pos, sizeof(CVector));
	CActorPool* pActorPool = pNetGame->GetActorPool();
#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("Set actor pos %d %f %f %f", actorId, pos.x, pos.y, pos.z);
#endif
	if (pActorPool)
	{
		if (pActorPool->GetAt(actorId))
		{
			pActorPool->GetAt(actorId)->TeleportTo(pos.x, pos.y, pos.z);
		}
	}
}

void SetActorFacingAngle(RPCParameters* rpcParams)
{
	

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	float angle;
	bsData.Read(actorId);
	bsData.Read(angle);
	CActorPool* pActorPool = pNetGame->GetActorPool();
#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("Apply actor facing %d %f", actorId, angle);
#endif
	if (pActorPool)
	{
		if (pActorPool->GetAt(actorId))
		{
			pActorPool->GetAt(actorId)->ForceTargetRotation(angle);
		}
	}
}

void SetActorAnimation(RPCParameters* rpcParams)
{
	

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	PLAYERID actorId;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	char szAnimLib[256];
	char szAnimName[256];
	float fS;
	bool opt1, opt2, opt3, opt4;
	int opt5;

	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	bsData.Read(actorId);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fS);
	bsData.Read(opt1);
	bsData.Read(opt2);
	bsData.Read(opt3);
	bsData.Read(opt4);
	bsData.Read(opt5);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';
#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("Apply actor animation %d %s %s %f", actorId, szAnimName, szAnimLib, fS);
#endif
	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (pActorPool)
	{
		if (pActorPool->GetAt(actorId))
		{
			pActorPool->GetAt(actorId)->ApplyAnimation(szAnimName, szAnimLib, fS, (int)opt1, (int)opt2, (int)opt3, (int)opt4, opt5);
		}
	}
}

void ClearActorAnimations(RPCParameters* rpcParams)
{
	
	
	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	uint16_t actorId;
	float angle;
	bsData.Read(actorId);
	CActorPool* pActorPool = pNetGame->GetActorPool();
#ifdef _CDEBUG
	CChatWindow::AddDebugMessage("Clear actor animation %d", actorId);
#endif
	if (pActorPool)
	{
		if (pActorPool->GetAt(actorId))
		{
			MATRIX4X4 mat;
			pActorPool->GetAt(actorId)->GetMatrix(&mat);
			pActorPool->GetAt(actorId)->TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
		}
	}
}

void UpdateScoresPingsIPs(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID bytePlayerId;
	int iPlayerScore;
	uint32_t dwPlayerPing;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	for (PLAYERID i = 0; i < (iBitLength / 8) / 9; i++)
	{
		bsData.Read(bytePlayerId);
		bsData.Read(iPlayerScore);
		bsData.Read(dwPlayerPing);

		pPlayerPool->UpdateScore(bytePlayerId, iPlayerScore);
		pPlayerPool->UpdatePing(bytePlayerId, dwPlayerPing);
	}
	CTab::update();
}

void RegisterRPCs(RakClientInterface* pRakClient)
{
	Log("Registering RPC's..");

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditAttachedObject, EditAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_InitGame, InitGame);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ServerJoin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ServerQuit);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ClientMessage);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Chat, Chat);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RequestClass);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RequestSpawn);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Weather, Weather);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldTime, WorldTime);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, SetTimeEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, WorldPlayerAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, WorldPlayerRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, SetCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, DisableCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, SetRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, DisableRaceCheckpoint);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, WorldVehicleAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, WorldVehicleRemove);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, EnterVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ExitVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, CDialog::rpcShowPlayerDialog);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_GameModeRestart, GameModeRestart);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ConnectionRejected);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Pickup, Pickup);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, DestroyPickup);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, Create3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDestroy3DTextLabel, Delete3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ScmEvent);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RemoveBuilding, RemoveBuilding);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetPlayerChatBubble, SetPlayerChatBubble);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, WorldPlayerDeath);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_VehicleDamage, DamageVehicle);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ShowActor, WorldActorAdd);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_HideActor, WorldActorRemove);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorAnimation, SetActorAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle, SetActorFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorPos, SetActorPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetActorHealth, SetActorHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClearActorAnimations, ClearActorAnimations);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, UpdateScoresPingsIPs);
}

void UnRegisterRPCs(RakClientInterface* pRakClient)
{
	Log("UnRegistering RPC's..");
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_InitGame);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerJoin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerQuit);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClientMessage);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Chat);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Weather);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldTime);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetTimeEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDialogBox);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_GameModeRestart);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ConnectionRejected);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Pickup);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DestroyPickup);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDestroy3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScmEvent);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RemoveBuilding);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetPlayerChatBubble);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_VehicleDamage);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ShowActor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_HideActor);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorAnimation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorFacingAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetActorHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClearActorAnimations);
}
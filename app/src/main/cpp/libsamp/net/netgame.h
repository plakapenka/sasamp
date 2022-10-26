#pragma once

// raknet
#include "../vendor/raknet/RakClientInterface.h"
#include "../vendor/raknet/RakNetworkFactory.h"
#include "../vendor/raknet/PacketEnumerations.h"
#include "../vendor/raknet/StringCompressor.h"

#include "localplayer.h"
#include "remoteplayer.h"
#include "playerpool.h"
#include "vehiclepool.h"
#include "gangzonepool.h"
#include "objectpool.h"
#include "pickuppool.h"
#include "textlabelpool.h"
#include "textdrawpool.h"
#include "streampool.h"
#include "CActorPool.h"

#define GAMESTATE_WAIT_CONNECT	9
#define GAMESTATE_CONNECTING	13
#define GAMESTATE_AWAIT_JOIN	15
#define GAMESTATE_CONNECTED 	14
#define GAMESTATE_RESTARTING	18
#define GAMESTATE_NONE 			0
#define GAMESTATE_DISCONNECTED	4

#define NETMODE_ONFOOT_SENDRATE	30
#define NETMODE_INCAR_SENDRATE	30
#define NETMODE_FIRING_SENDRATE 30
#define NETMODE_SEND_MULTIPLIER 2

#define INVALID_PLAYER_ID 0xFFFF
#define INVALID_VEHICLE_ID 0xFFFF


#define AUTH_SALT "78r5jgwvbcg6dv1"

// Packet 251
#define RPC_TOGGLE_HUD_ELEMENT 1
#define RPC_STREAM_CREATE 2
#define RPC_STREAM_POS 3
#define RPC_STREAM_DESTROY 4
#define RPC_STREAM_INDIVIDUAL 5
#define RPC_STREAM_VOLUME 6
#define RPC_STREAM_ISENABLED 7
#define RPC_OPEN_LINK 8
#define RPC_TIMEOUT_CHAT 9
#define RPC_CUSTOM_COMPONENT 10
#define RPC_CUSTOM_HANDLING 11
#define RPC_CUSTOM_VISUALS 13
#define RPC_CUSTOM_HANDLING_DEFAULTS 14
#define RPC_OPEN_SETTINGS 15
#define RPC_CUSTOM_SET_FUEL 16
#define RPC_CUSTOM_SET_LEVEL 17
#define RPC_TOGGLE_GPS_INFO 18
#define RPC_TOGGLE_GREENZONE 19
#define RPC_TOGGLE_SAMWILL_GAME 20 // �������� uint8 toggle, ��������� uint8 quantity
#define RPC_KILLLIST 21
#define RPC_CLEAR_KILLLIST 22
#define RPC_SHOW_DUELS_KILLS_LEFT 23
#define RPC_DUELS_SHOW_LOCAL_TOP 24
#define RPC_DUELS_SHOW_LOCAL_STAT 25
#define RPC_SHOW_ACTION_LABEL 26
#define RPC_SHOW_TARGET_LABEL 27
#define RPC_SHOW_FACTORY_GAME 28
#define RPC_SHOW_MINING_GAME 29
#define RPC_SHOW_TD_BUS 30
#define RPC_SHOW_LUCKYWHEEL 32
#define RPC_SHOW_OILGAME 33
#define RPC_SHOW_DICE_TABLE 31
#define RPC_SHOW_CASINO_BUY_CHIP 34
#define RPC_SHOW_AUTOSHOP 35
#define RPC_UPDATE_AUTOSHOP 36
#define RPC_CLICK_AUTOSHOP 37
#define RPC_FUELSTATION_BUY 38
#define RPC_FUELSTATION_SEND 39
#define RPC_TOGGLE_ACCESSORIES_MENU 40
#define RPC_TOGGLE_CLOTHING_MENU 41
#define RPC_SHOPSTORE_BUTTON 42
#define RPC_GUNSTORE_TOGGLE 43
#define RPC_GUNSTORETOGGLESEND 44
#define RPC_SHOW_ARMY_GAME 45
#define RPC_CHECK_CLIENT 46
#define RPC_SHOW_CONTEINER_AUC 52
#define RPC_PRE_DEATH 48

// Packet 252
#define RPC_TOGGLE_LOGIN (1)
#define RPC_TOGGLE_REGISTER (2)

// Packet 253
#define RPC_TOGGLE_CHOOSE_SPAWN (2)

// other
//#define RPC_CUSTOM_ACTOR_PUT_IN_VEH 0x20
//#define RPC_CUSTOM_ACTOR_REMOVE_VEH 0x21
//#define RPC_CUSTOM_ACTOR_ADD_ADDITIONAL 0x22

class CNetGame
{
public:
	CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass);
	~CNetGame();

	void Process();

	CPlayerPool* GetPlayerPool() { return m_pPlayerPool; }
	CVehiclePool* GetVehiclePool() { return m_pVehiclePool; }
	CObjectPool* GetObjectPool() { return m_pObjectPool; }
	CPickupPool* GetPickupPool() { return m_pPickupPool; }
	CGangZonePool* GetGangZonePool() { return m_pGangZonePool; }
	CText3DLabelsPool* GetLabelPool() { return m_pLabelPool; }
	CTextDrawPool* GetTextDrawPool() { return m_pTextDrawPool; }
	CStreamPool* GetStreamPool() { return m_pStreamPool; }
	CActorPool* GetActorPool() { return m_pActorPool; }
	RakClientInterface* GetRakClient() { return m_pRakClient; };

	int GetGameState() { return m_iGameState; }
	void SetGameState(int iGameState) { m_iGameState = iGameState; }

	void ResetVehiclePool();
	void ResetObjectPool();
	void ResetPickupPool();
	void ResetGangZonePool();
	void ResetLabelPool();
	void ResetActorPool();
	void ShutDownForGameRestart();
	void ResetTextDrawPool();

	void SendChatMessage(const char* szMsg);
	void SendChatCommand(const char* szMsg);
	void SendDialogResponse(uint16_t wDialogID, uint8_t byteButtonID, uint16_t wListboxItem, char* szInput);
	void SendCustomPacket(uint8_t packet, uint8_t RPC, uint8_t Quantity);
	void SendCustomCasinoChipPacket(uint8_t packet, uint8_t RPC, uint8_t type, uint8_t button, uint32_t money);
	void SendCustomPacketFuelData(uint8_t packet, uint8_t RPC, uint8_t fueltype, uint32_t fuel);
	void SendLoginPacket(char *password);
	void SendCheckClientPacket(const char password[]);
	void SendSpeedTurnPacket(uint8_t turnId, uint8_t state);
	void SendRegisterPacket(char *password, char *mail, uint8_t sex, uint8_t skin);
	void SendRegisterSkinPacket(uint32_t skinId);
	void SendNotifyButtonPacket(uint16_t actionId, uint8_t buttonId);

	void SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style);
	void DisableMapIcon(uint8_t byteIndex);

	void UpdatePlayerScoresAndPings();
private:
	RakClientInterface* m_pRakClient;
	CPlayerPool*		m_pPlayerPool;
	CVehiclePool*		m_pVehiclePool;
	CObjectPool*		m_pObjectPool;
	CPickupPool* 		m_pPickupPool;
	CGangZonePool*		m_pGangZonePool;
	CText3DLabelsPool*	m_pLabelPool;
	CTextDrawPool*		m_pTextDrawPool;
	CStreamPool*		m_pStreamPool;
	CActorPool*			m_pActorPool;
	int					m_iGameState;
	uint32_t			m_dwLastConnectAttempt;

	uint32_t			m_dwMapIcons[100];

	void UpdateNetwork();
	void Packet_AuthKey(Packet *p);
	void Packet_DisconnectionNotification(Packet *p);
	void Packet_ConnectionLost(Packet *p);
	void Packet_ConnectionSucceeded(Packet *p);
	void Packet_PlayerSync(Packet* pkt);
	void Packet_VehicleSync(Packet* pkt);
	void Packet_PassengerSync(Packet* pkt);
	void Packet_MarkersSync(Packet* pkt);
	void Packet_AimSync(Packet* p);
	void Packet_BulletSync(Packet* pkt);
	void Packet_TrailerSync(Packet* p);
	void Packet_CustomRPC(Packet* p);
	void Packet_SpecialCustomRPC(Packet* p);
	void Packet_AuthRPC(Packet* p);

public:
	char m_szHostName[0xFF];
	char m_szHostOrIp[0x7F];
	int m_iPort;

	bool 		m_CasinoDiceLayoutState = false;
	bool		m_GreenZoneState;
	bool		m_bZoneNames;
	bool		m_bUseCJWalk;
	bool		m_bAllowWeapons;
	bool		m_bLimitGlobalChatRadius;
	float		m_fGlobalChatRadius;
	float 		m_fNameTagDrawDistance;
	bool		m_bDisableEnterExits;
	bool		m_bNameTagLOS;
	bool		m_bManualVehicleEngineAndLight;
	int 		m_iSpawnsAvailable;
	bool 		m_bShowPlayerTags;
	int 		m_iShowPlayerMarkers;
	bool		m_bHoldTime;
	uint8_t		m_byteWorldTime;
	uint8_t		m_byteWorldMinute;
	uint8_t		m_byteWeather;

	uint8_t* m_bChecked;

	float 		m_fGravity;
	int 		m_iDeathDropMoney;
	bool 		m_bInstagib;
	int 		m_iLagCompensation;
	int 		m_iVehicleFriendlyFire;
};

extern CNetGame *pNetGame;
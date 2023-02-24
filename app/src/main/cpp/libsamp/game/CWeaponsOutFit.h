#pragma once

class CWeaponsOutFit
{
	struct SWeaponOutFitSettings
	{
	public:
		int iSlot;
		int iBone;
		CVector vOffset;
		CVector vRotation;

		bool bUseSecondary;
		int iBoneSecondary;
		CVector vOffsetSecondary;
		CVector vRotationSecondary;

		SWeaponOutFitSettings()
		{
			bUseSecondary = false;
			iBone = -1;
			iBoneSecondary = -1;

			vOffset.x = 0.0f;
			vOffset.x = 0.0f;
			vOffset.z = 0.0f;

			vRotation.x = 0.0f;
			vRotation.y = 0.0f;
			vRotation.z = 0.0f;

			vOffsetSecondary.x = 0.0f;
			vOffsetSecondary.y = 0.0f;
			vOffsetSecondary.z = 0.0f;

			vRotationSecondary.x = 0.0f;
			vRotationSecondary.y = 0.0f;
			vRotationSecondary.z = 0.0f;
		}
	};

	static uint32_t m_bLastTickUpdate;
	static bool m_bUpdated;
	static SWeaponOutFitSettings m_Settings[MAX_WEAPON_MODELS];
	static bool m_bUsedWeapon[MAX_WEAPON_MODELS];
	static int m_iSlots[MAX_WEAPON_MODELS];
	static bool m_bUsedSlots[10];
	static bool m_bEnabled;
	static bool m_bParsed;

	static int GetWeaponModelIDFromSlot(int iSlot);

	static void OnWeaponAdded(CPlayerPed* pPed, int iWeaponID);
	static void OnWeaponRemoved(CPlayerPed* pPed, int iWeaponID);
	static void OnPedDie(CPlayerPed* pPed);
public:
	static void ParseDatFile();
	static void ProcessLocalPlayer(CPlayerPed* pPed);

	static void SetEnabled(bool bEnabled);

	static void OnUpdateOffsets();
};
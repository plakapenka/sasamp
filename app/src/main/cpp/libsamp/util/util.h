#pragma once


#include "game/CVehicleGta.h"
#include "game/CPedGta.h"

static class CUtil {
public:
    static bool IsValidGameVehicle(CVehicleGta *pVehicle);
    static bool IsValidGamePed(CPedGta * pPed);

    static bool IsGameEntityArePlaceable(CEntityGta *pEntity);

    static void WorldRemoveEntity(uintptr_t pEnt);

    static const char *GetWeaponName(int iWeaponID);

    static RwTexture *LoadTextureFromDB(const char *dbname, const char *texture);

    static uintptr_t FindLibrary(const char *library);

    static int32_t FindPlayerSlotWithPedPointer(CPedGta *pPlayersPed);

    static CPedGta *FindPlayerPed(int32_t Player);

    static CPhysicalGta *GetPoolObj(int slot);

    static CPedGta *GetPoolPed(int slot);

    static RwTexture *GetTexture(const char *name);

    static float DegToRad(float fDegrees);

    constexpr static float RadiansToDegrees(float angleInRadians) {
        return angleInRadians * 180.0F / PI;
    }
};

void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);

void AND_OpenLink(const char* szLink);
char* ConvertColorToHtml(std::string format);
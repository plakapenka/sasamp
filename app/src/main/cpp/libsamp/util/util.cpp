#include "../main.h"
#include <vector>

bool CUtil::IsValidGameVehicle(VEHICLE_TYPE *pVehicle)
{
	// IsVehiclePointerValid
	return (((bool (*)(VEHICLE_TYPE *))(g_libGTASA+0x5109E8+1))(pVehicle));
}

bool CUtil::IsValidGamePed(PED_TYPE * pPed)
{
	// IsPedPointerValid(CPed *) — 0x00435614
	return ((bool(*)(PED_TYPE *)) (g_libGTASA + 0x00435614 + 1)) (pPed);
}

bool CUtil::IsGameEntityArePlaceable(ENTITY_TYPE *pEntity)
{
	if(pEntity)
	{
		if(pEntity->vtable == g_libGTASA + 0x005C7358)
			return true;
	}

	return false;
}

const char* CUtil::GetWeaponName(int iWeaponID)
{
	switch (iWeaponID)
	{
		case WEAPON_FIST:
			return "Fist";
		case WEAPON_BRASSKNUCKLE:
			return "Brass Knuckles";
		case WEAPON_GOLFCLUB:
			return "Golf Club";
		case WEAPON_NITESTICK:
			return "Nite Stick";
		case WEAPON_KNIFE:
			return "Knife";
		case WEAPON_BAT:
			return "Baseball Bat";
		case WEAPON_SHOVEL:
			return "Shovel";
		case WEAPON_POOLSTICK:
			return "Pool Cue";
		case WEAPON_KATANA:
			return "Katana";
		case WEAPON_CHAINSAW:
			return "Chainsaw";
		case WEAPON_DILDO:
			return "Dildo";
		case WEAPON_DILDO2:
			return "Dildo";
		case WEAPON_VIBRATOR:
			return "Vibrator";
		case WEAPON_VIBRATOR2:
			return "Vibrator";
		case WEAPON_FLOWER:
			return "Flowers";
		case WEAPON_CANE:
			return "Cane";
		case WEAPON_GRENADE:
			return "Grenade";
		case WEAPON_TEARGAS:
			return "Teargas";
		case WEAPON_MOLTOV:
			return "Molotov";
		case WEAPON_COLT45:
			return "Colt 45";
		case WEAPON_SILENCED:
			return "Silenced Pistol";
		case WEAPON_DEAGLE:
			return "Desert Eagle";
		case WEAPON_SHOTGUN:
			return "Shotgun";
		case WEAPON_SAWEDOFF:
			return "Sawn-off Shotgun";
		case WEAPON_SHOTGSPA: // wtf?
			return "Combat Shotgun";
		case WEAPON_UZI:
			return "UZI";
		case WEAPON_MP5:
			return "MP5";
		case WEAPON_AK47:
			return "AK47";
		case WEAPON_M4:
			return "M4";
		case WEAPON_TEC9:
			return "TEC9";
		case WEAPON_RIFLE:
			return "Rifle";
		case WEAPON_SNIPER:
			return "Sniper Rifle";
		case WEAPON_ROCKETLAUNCHER:
			return "Rocket Launcher";
		case WEAPON_HEATSEEKER:
			return "Heat Seaker";
		case WEAPON_FLAMETHROWER:
			return "Flamethrower";
		case WEAPON_MINIGUN:
			return "Minigun";
		case WEAPON_SATCHEL:
			return "Satchel Explosives";
		case WEAPON_BOMB:
			return "Bomb";
		case WEAPON_SPRAYCAN:
			return "Spray Can";
		case WEAPON_FIREEXTINGUISHER:
			return "Fire Extinguisher";
		case WEAPON_CAMERA:
			return "Camera";
		case WEAPON_PARACHUTE:
			return "Parachute";
		case WEAPON_VEHICLE:
			return "Vehicle";
		case WEAPON_HELIBLADES:
			return "Heli blades";
		case WEAPON_EXPLOSION:
			return "Explosion";
		case WEAPON_DROWN:
			return "Drowned";
		case WEAPON_COLLISION:
			return "Splat";
		default:
			return "unknown";
	}
}

uintptr_t FindLibrary(const char* library)
{
	char filename[0xFF] = { 0 },
		buffer[2048] = { 0 };
	FILE* fp = 0;
	uintptr_t address = 0;

	sprintf(filename, "/proc/%d/maps", getpid());

	fp = fopen(filename, "rt");
	if (fp == 0)
	{
		Log("ERROR: can't open file %s", filename);
		goto done;
	}

	while (fgets(buffer, sizeof(buffer), fp))
	{
		if (strstr(buffer, library))
		{
			address = (uintptr_t)strtoul(buffer, 0, 16);
			break;
		}
	}

done:

	if (fp)
		fclose(fp);

	return address;
}
void CrashLog(const char* fmt, ...);

#include <algorithm>

#include "..//str_obfuscator_no_template.hpp"

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen


void cp1251_to_utf8(char* out, const char* in, unsigned int len)
{
	static const int table[128] =
	{
		// 80
		0x82D0,     0x83D0,     0x9A80E2,   0x93D1,     0x9E80E2,   0xA680E2,   0xA080E2,   0xA180E2,
		0xAC82E2,   0xB080E2,   0x89D0,     0xB980E2,   0x8AD0,     0x8CD0,     0x8BD0,     0x8FD0,
		// 90
		0x92D1,     0x9880E2,   0x9980E2,   0x9C80E2,   0x9D80E2,   0xA280E2,   0x9380E2,   0x9480E2,
		0,          0xA284E2,   0x99D1,     0xBA80E2,   0x9AD1,     0x9CD1,     0x9BD1,     0x9FD1,
		// A0
		0xA0C2,     0x8ED0,     0x9ED1,     0x88D0,     0xA4C2,     0x90D2,     0xA6C2,     0xA7C2,
		0x81D0,     0xA9C2,     0x84D0,     0xABC2,     0xACC2,     0xADC2,     0xAEC2,     0x87D0,
		// B0
		0xB0C2,     0xB1C2,     0x86D0,     0x96D1,     0x91D2,     0xB5C2,     0xB6C2,     0xB7C2,
		0x91D1,     0x9684E2,   0x94D1,     0xBBC2,     0x98D1,     0x85D0,     0x95D1,     0x97D1,
		// C0
		0x90D0,     0x91D0,     0x92D0,     0x93D0,     0x94D0,     0x95D0,     0x96D0,     0x97D0,
		0x98D0,     0x99D0,     0x9AD0,     0x9BD0,     0x9CD0,     0x9DD0,     0x9ED0,     0x9FD0,
		// D0
		0xA0D0,     0xA1D0,     0xA2D0,     0xA3D0,     0xA4D0,     0xA5D0,     0xA6D0,     0xA7D0,
		0xA8D0,     0xA9D0,     0xAAD0,     0xABD0,     0xACD0,     0xADD0,     0xAED0,     0xAFD0,
		// E0
		0xB0D0,     0xB1D0,     0xB2D0,     0xB3D0,     0xB4D0,     0xB5D0,     0xB6D0,     0xB7D0,
		0xB8D0,     0xB9D0,     0xBAD0,     0xBBD0,     0xBCD0,     0xBDD0,     0xBED0,     0xBFD0,
		// F0
		0x80D1,     0x81D1,     0x82D1,     0x83D1,     0x84D1,     0x85D1,     0x86D1,     0x87D1,
		0x88D1,     0x89D1,     0x8AD1,     0x8BD1,     0x8CD1,     0x8DD1,     0x8ED1,     0x8FD1
	};

	int count = 0;

	while (*in)
	{
		if (len && (count >= len)) break;

		if (*in & 0x80)
		{
			int v = table[(int)(0x7f & *in++)];
			if (!v)
				continue;
			*out++ = (char)v;
			*out++ = (char)(v >> 8);
			if (v >>= 16)
				* out++ = (char)v;
		}
		else // ASCII
			*out++ = *in++;

		count++;
	}

	*out = 0;
}

void AND_OpenLink(const char* szLink)
{
	((void (*)(const char*))(g_libGTASA + 0x00242A64 + 1))(szLink);
}

char* ConvertColorToHtml(std::string format) {

	bool first_time_flag = 1;
	for (int i = 0; i < format.length(); i++) {
		if (format[i] == '{') {

			first_time_flag ? format.insert(++i, "<font color='#") : format.insert(++i, "</font><font color='#");
		}
		if (format[i] == '}') {
			format.insert(++i, "'>");
			first_time_flag = 0;
		}
	}

	format.erase(remove(format.begin(), format.end(), '{'), format.end());
	format.erase(remove(format.begin(), format.end(), '}'), format.end());
	format.append("</font>");

	for(int i = 0; i <= format.size()-10; i++)
	{
		if(format[i] == '\n')
		{
			//  cout << gg;
			//gg.replace(i, "<br>");
			format[i] = '<';
			format.insert(i+1, "br>");
			i+=3;
		}
	}
	return const_cast<char *>(format.c_str());
}
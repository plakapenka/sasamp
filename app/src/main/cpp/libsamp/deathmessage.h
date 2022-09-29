#pragma once

#include <list>
#include <string>

struct DeathMessageStruct
{
	std::string playerName;
	unsigned int playercolor;
	std::string killerName;
	unsigned int killercolor;
	uint8_t reason;
};

class CDeathMessage
{
public:
	CDeathMessage();
	~CDeathMessage();

	void MakeRecord(const char* playername, unsigned int playercolor, const char* killername, unsigned int killercolor, uint8_t reason);
	void Render();

private:
	const char* SpriteIDForWeapon(uint8_t byteWeaponID);

private:
	std::list<DeathMessageStruct*> m_pDeathMessage;
};
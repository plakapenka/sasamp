#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
		CServerInstance::create("94.250.248.153", 1, 20, 7777, false),
	CServerInstance::create("46.174.49.47", 1, 20, 7788, false),	// москва
	CServerInstance::create("46.174.49.47", 1, 20, 7825, false), 	// спб
	CServerInstance::create("46.174.49.47", 1, 20, 7786, false),	// тест
	CServerInstance::create("46.174.49.47", 1, 20, 7830, false),	// samara
	CServerInstance::create("185.43.4.242", 1, 20, 7777, false)
};

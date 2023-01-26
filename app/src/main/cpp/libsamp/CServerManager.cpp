#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("82.202.173.174", 1, 20, 1233, false),	// основа
    CServerInstance::create("94.250.248.153", 1, 20, 7788, false)		// release
};


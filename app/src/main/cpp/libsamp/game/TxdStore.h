//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_TXDSTORE_H
#define LIVERUSSIA_TXDSTORE_H


#include "game/RW/rwcore.h"
#include "TxdDef.h"
#include "Core/Pool.h"

struct _TxdParent {
    RwTexDictionary* parent;
};

/**
 * Txd Store plugin unique rwID
 */
#define rwID_TXDPARENTPLUGIN  MAKECHUNKID(rwVENDORID_DEVELOPER, 0xF5)

static inline int32_t& ms_txdPluginOffset = *reinterpret_cast<int32_t*>(0xC88018);
typedef CPool<TxdDef> CTxdPool;

class CTxdStore {
public:
    static CTxdPool*&           ms_pTxdPool;
    static RwTexDictionary*&    ms_pStoredTxd;
    static int32_t&             ms_lastSlotFound;
    // variables list is not finished. Need to make CPools before.

    static int16_t (&defaultTxds)[4];

public:
    static bool PluginAttach();
    static void Initialise();
    static void Shutdown();
    static void GameShutdown();

    static bool StartLoadTxd(int32_t index, RwStream* stream);
    static bool FinishLoadTxd(int32_t index, RwStream* stream);
    static bool LoadTxd(int32_t index, RwStream* stream);
    static bool LoadTxd(int32_t index, const char* filename);

    static void PushCurrentTxd();
    static void PopCurrentTxd();
    static void SetCurrentTxd(int32_t index);

    static int32_t FindTxdSlot(const char* name);
    static int32_t FindTxdSlot(uint32_t hash);

    static RwTexDictionary* GetTxd(int32_t index);
    static int32_t GetParentTxdSlot(int32_t index);

    static void Create(int32_t index);

    static int32_t AddTxdSlot(const char* name);
    static void RemoveTxdSlot(int32_t index);
    static void RemoveTxd(int32_t index);

    static void AddRef(int32_t index);
    static void RemoveRef(int32_t index);
    static void RemoveRefWithoutDelete(int32_t index);
    static int32_t GetNumRefs(int32_t index);

    static RwTexDictionary* GetTxdParent(RwTexDictionary* txd);
    static void SetTxdParent(RwTexDictionary* txd, RwTexDictionary* parent);
    static void SetupTxdParent(int32_t index);

    static RwTexture* TxdStoreFindCB(const char* name);
    static RwTexture* TxdStoreLoadCB(const char* name, const char* mask);

    static auto FindOrAddTxdSlot(const char* name) {
        auto slot = CTxdStore::FindTxdSlot(name);
        if (slot == -1) slot = CTxdStore::AddTxdSlot(name);
        return slot;
    }
    static void SafeRemoveTxdSlot(const char* name) {
        auto slot = CTxdStore::FindTxdSlot(name);
        if (slot != -1) CTxdStore::RemoveTxdSlot(slot);
    }
};

RwTexture* RemoveIfRefCountIsGreaterThanOne(RwTexture* texture, void* data);

#endif //LIVERUSSIA_TXDSTORE_H

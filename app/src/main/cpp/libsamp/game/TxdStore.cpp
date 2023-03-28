//
// Created by plaka on 28.03.2023.
//

#include "TxdStore.h"
#include "constants.h"
#include "game/RW/rwtexdict.h"
#include "Streaming.h"
#include "game/RW/RenderWare.h"
#include "game/Core/KeyGen.h"

// initialise txd store
// 0x731F20
void CTxdStore::Initialise() {
    if (!ms_pTxdPool)
        ms_pTxdPool = new CTxdPool(TOTAL_TXD_MODEL_IDS, "TexDictionary");

    for (auto& txd : defaultTxds)
        txd = static_cast<int16_t>(AddTxdSlot("*"));

    RwTextureSetFindCallBack(TxdStoreFindCB);
    RwTextureSetReadCallBack(TxdStoreLoadCB);
}

// shutdown txd store
// 0x732000
void CTxdStore::Shutdown() {
    delete ms_pTxdPool;
}

// 0x732060
void CTxdStore::GameShutdown() {
    for (int32_t i = 0; i < TOTAL_TXD_MODEL_IDS; i++) {
        TxdDef* txd = ms_pTxdPool->GetAt(i);
        if (!txd || txd->m_wRefsCount)
            continue;
        RemoveTxdSlot(i);
    }
}

// 0x731930
bool CTxdStore::StartLoadTxd(int32_t index, RwStream* stream) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd || !RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr))
        return false;

    RwTexDictionary* texdic = RwTexDictionaryGtaStreamRead1(stream);
    txd->m_pRwDictionary = texdic;
    return texdic != nullptr;
}

// 0x731E40
bool CTxdStore::FinishLoadTxd(int32_t index, RwStream* stream) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return false;

    txd->m_pRwDictionary = RwTexDictionaryGtaStreamRead2(stream, txd->m_pRwDictionary);
    if (txd->m_pRwDictionary)
        SetupTxdParent(index);
    return txd->m_pRwDictionary != nullptr;
}

// load txd from stream
// 0x731DD0
bool CTxdStore::LoadTxd(int32_t index, RwStream* stream) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd || !RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr))
        return false;
    txd->m_pRwDictionary = RwTexDictionaryGtaStreamRead(stream);
    if (txd->m_pRwDictionary)
        SetupTxdParent(index);
    return txd->m_pRwDictionary != nullptr;
}

// load txd from file
// 0x7320B0
bool CTxdStore::LoadTxd(int32_t index, const char* filename) {
    char data[260];
    RwStream* stream;
    sprintf(data, "%s", filename);
    do {
        stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, data);
    } while (!stream);
    bool loaded = LoadTxd(index, stream);
    RwStreamClose(stream, nullptr);
    return loaded;
}

// store current txd
// 0x7316A0
void CTxdStore::PushCurrentTxd() {
    ms_pStoredTxd = RwTexDictionaryGetCurrent();
}

// restore txd
// 0x7316B0
void CTxdStore::PopCurrentTxd() {
    RwTexDictionarySetCurrent(ms_pStoredTxd);
    ms_pStoredTxd = nullptr;
}

// set the txd to work with
// 0x7319C0
void CTxdStore::SetCurrentTxd(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        RwTexDictionarySetCurrent(txd->m_pRwDictionary);
}

// find txd by name. Returning value is txd index
// 0x731850
int32_t CTxdStore::FindTxdSlot(const char* name) {
    uint32_t key = CKeyGen::GetUppercaseKey(name);
    int32_t last = ms_lastSlotFound;

    for (; last >= 0; last--) {
        TxdDef* txd = ms_pTxdPool->GetAt(last);
        if (txd && txd->m_hash == key)
            break;
    }

    if (last < 0) {
        last = ms_lastSlotFound;
        for (last++;; last++) {
            if (last >= ms_pTxdPool->GetSize())
                return -1;

            TxdDef* txd = ms_pTxdPool->GetAt(last);
            if (txd && txd->m_hash == key)
                break;
        }
    }

    ms_lastSlotFound = last;
    return last;
}

// find txd by name hash. Returning value is txd index
// 0x7318E0
int32_t CTxdStore::FindTxdSlot(uint32_t hash) {
    for (int32_t i = 0; i < ms_pTxdPool->GetSize(); i++) {
        TxdDef* txd = ms_pTxdPool->GetAt(i);
        if (txd && txd->m_hash == hash)
            return i;
    }
    return -1;
}

// plugin-sdk has named it to getTexDictionary
// 0x408340
RwTexDictionary* CTxdStore::GetTxd(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    return txd ? txd->m_pRwDictionary : nullptr;
}

// plugin-sdk has named it to getTXDEntryParentIndex
// 0x408370
int32_t CTxdStore::GetParentTxdSlot(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    return txd ? txd->m_wParentIndex : -1;
}

// create rw tex dictionary for txd slot
// 0x731990
void CTxdStore::Create(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        txd->m_pRwDictionary = RwTexDictionaryCreate();
}

// allocate new slot for this txd
// 0x731C80
int32_t CTxdStore::AddTxdSlot(const char* name) {
    TxdDef* txd = ms_pTxdPool->New();
    txd->m_pRwDictionary = nullptr;
    txd->m_wRefsCount = 0;
    txd->m_wParentIndex = -1;
    txd->m_hash = CKeyGen::GetUppercaseKey(name);

    return ms_pTxdPool->GetIndex(txd);
}

// remove txd slot
// 0x731CD0
void CTxdStore::RemoveTxdSlot(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (txd->m_pRwDictionary)
        RwTexDictionaryDestroy(txd->m_pRwDictionary);
    ms_pTxdPool->Delete(txd);
}

// remove txd
// 0x731E90
void CTxdStore::RemoveTxd(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (txd->m_pRwDictionary) {
        RwTexDictionaryForAllTextures(txd->m_pRwDictionary, RemoveIfRefCountIsGreaterThanOne, nullptr);
        RwTexDictionaryDestroy(txd->m_pRwDictionary);
    }
    if (txd->m_wParentIndex != -1)
        RemoveRef(txd->m_wParentIndex);

    txd->m_pRwDictionary = nullptr;
}

// add reference for txd
// 0x731A00
void CTxdStore::AddRef(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        txd->m_wRefsCount++;
}

// remove reference, if references count is 0, then delete txd
// 0x731A30
void CTxdStore::RemoveRef(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (!txd)
        return;

    if (--txd->m_wRefsCount <= 0)
        CStreaming::RemoveModel(TXDToModelId(index));
}

// remove reference without deleting
// 0x731A70
void CTxdStore::RemoveRefWithoutDelete(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    if (txd)
        txd->m_wRefsCount--;
}

// get references count
// 0x731AA0
int32_t CTxdStore::GetNumRefs(int32_t index) {
    TxdDef* txd = ms_pTxdPool->GetAt(index);
    return txd ? txd->m_wRefsCount : 0;
}

// 0x731D50
void CTxdStore::SetupTxdParent(int32_t index) {
    TxdDef* txdcur = ms_pTxdPool->GetAt(index);
    if (!txdcur || txdcur->m_wParentIndex == -1)
        return;
    TxdDef* txdpar = ms_pTxdPool->GetAt(txdcur->m_wParentIndex);
    if (!txdpar)
        return;

    SetTxdParent(txdcur->m_pRwDictionary, txdpar->m_pRwDictionary);
    AddRef(txdcur->m_wParentIndex);
}

// 0x731720
RwTexture* CTxdStore::TxdStoreFindCB(const char* name) {
    RwTexDictionary* txd = RwTexDictionaryGetCurrent();
    for (; txd; txd = GetTxdParent(txd)) {
        RwTexture* tex = RwTexDictionaryFindNamedTexture(txd, name);
        if (tex)
            return tex;
    }
    return nullptr;
}

// 0x731710
RwTexture* CTxdStore::TxdStoreLoadCB(const char* name, const char* mask) {
    return nullptr;
}

// 0x731680
RwTexture* RemoveIfRefCountIsGreaterThanOne(RwTexture* texture, void* data) {
    if (texture->refCount > 1) {
        RwTextureDestroy(texture);
        RwTexDictionaryRemoveTexture(texture);
    }
    return texture;
}

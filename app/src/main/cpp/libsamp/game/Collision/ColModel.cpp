//
// Created by plaka on 29.03.2023.
//

#include "ColModel.h"

CColModel::~CColModel() {
    if (!m_bIsActive) {
        return;
    }
    RemoveCollisionVolumes();
}

void CColModel::RemoveCollisionVolumes() {
//    if (!m_pColData) {
//        return;
//    }
//
//    //Log("Removing: {} [ColSlot: {}]", LOG_PTR(this), m_nColSlot);
//
//    if (m_useSingleAlloc) {
//        CCollision::RemoveTrianglePlanes(m_pColData);
//        CMemoryMgr::Free(m_pColData);
//    } else {
//        m_pColData->RemoveCollisionVolumes();
//        delete m_pColData;
//    }
//
//    m_pColData = nullptr;
}
//
// Created by plaka on 23.03.2023.
//

#ifndef LIVERUSSIA_CMATRIX_H
#define LIVERUSSIA_CMATRIX_H


#include "CVector.h"


class CMatrix {

public:
    // RwV3d-like:
    CVector m_right;        // 0x0  // RW: Right
    uint32_t  flags;          // 0xC
    CVector m_forward;      // 0x10 // RW: Up
    uint32_t  pad1;           // 0x1C
    CVector m_up;           // 0x20 // RW: At
    uint32_t  pad2;           // 0x2C
    CVector m_pos;          // 0x30
    uint32_t  pad3;           // 0x3C

public:
    uintptr_t * m_pAttachMatrix{};       // 0x40
    bool      m_bOwnsAttachedMatrix{}; // 0x44 - Do we need to delete attached matrix at detaching

    CVector& GetRight() { return m_right; }
    const CVector& GetRight() const { return m_right; }

    CVector& GetForward() { return m_forward; }
    const CVector& GetForward() const { return m_forward; }

    CVector& GetUp() { return m_up; }
    const CVector& GetUp() const { return m_up; }

    CVector& GetPosition() { return m_pos; }
    const CVector& GetPosition() const { return m_pos; }
};


#endif //LIVERUSSIA_CMATRIX_H

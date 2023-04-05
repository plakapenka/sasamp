//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_PTRLIST_H
#define LIVERUSSIA_PTRLIST_H


#include <cstdint>
#include "PtrNode.h"

class CPtrList {
public:
    CPtrNode* m_node;

public:
    CPtrList();

    uint32_t CountElements() const;
    bool IsMemberOfList(void* data) const;

    CPtrNode* GetNode() const { return m_node; }
    bool IsEmpty() const { return !m_node; }
};
static_assert(sizeof(CPtrList) == 0x4, "Invalid");

#endif //LIVERUSSIA_PTRLIST_H

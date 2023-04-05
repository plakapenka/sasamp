//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_PTRLISTSINGLELINK_H
#define LIVERUSSIA_PTRLISTSINGLELINK_H


#include "PtrList.h"
#include "PtrNodeSingleLink.h"

class CPtrListSingleLink : public CPtrList {
public:
    ~CPtrListSingleLink() { Flush(); }

    static void InjectHooks();

    void Flush();
    CPtrNodeSingleLink* AddItem(void* item);
    void DeleteItem(void* item);
    void DeleteNode(CPtrNodeSingleLink* node, CPtrNodeSingleLink* lastNode); // Most likely inlined in the final exe,

    CPtrNodeSingleLink* GetNode() { return reinterpret_cast<CPtrNodeSingleLink*>(m_node); }
};


#endif //LIVERUSSIA_PTRLISTSINGLELINK_H

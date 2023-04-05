//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_PTRNODESINGLELINK_H
#define LIVERUSSIA_PTRNODESINGLELINK_H


class CPtrNodeSingleLink {
public:
    void*               m_item;
    CPtrNodeSingleLink* m_next;

public:
    static void InjectHooks();

    static void* operator new(unsigned size);
    static void  operator delete(void* data);

    CPtrNodeSingleLink(void* item) : m_item(item) {}

    void AddToList(class CPtrListSingleLink* list);
};


#endif //LIVERUSSIA_PTRNODESINGLELINK_H

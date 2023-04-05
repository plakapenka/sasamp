//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_PTRNODE_H
#define LIVERUSSIA_PTRNODE_H

class CPtrNode {
public:
    void*     m_item;
    CPtrNode* m_next;

    CPtrNode(void* item) : m_item(item) {}
    CPtrNode* GetNext() const { return m_next; }

    template<typename T>
    T* ItemAs() { return reinterpret_cast<T*>(m_item); }
};
static_assert(sizeof(CPtrNode) == 0x8, "Invalid");


#endif //LIVERUSSIA_PTRNODE_H

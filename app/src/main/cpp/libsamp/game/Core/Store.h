//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_STORE_H
#define LIVERUSSIA_STORE_H

#include <cstddef>

template <typename ObjectType, size_t Capacity> class CStore {
public:
    size_t     m_nCount;
    ObjectType m_aObjects[Capacity];

public:
    CStore() {
        m_nCount = 0;
    }

    ObjectType& GetItemAtIndex(size_t index) {
        return m_aObjects[index];
    }

    inline ObjectType& AddItem() {
        auto& obj = m_aObjects[m_nCount];
        ++m_nCount;
        return obj;
    }
};



#endif //LIVERUSSIA_STORE_H

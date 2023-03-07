//
// Created by plaka on 07.03.2023.
//

#ifndef LIVERUSSIA_CSTORE_H
#define LIVERUSSIA_CSTORE_H


template <typename ObjectType, unsigned int Capacity>
class CStore {
public:
    unsigned int m_nCount;
    ObjectType   m_aObjects[Capacity];

    CStore() {
        m_nCount = 0;
    }

    inline ObjectType& GetItemAtIndex(unsigned int index) { return m_aObjects[index]; }
    inline ObjectType& AddItem() {
        auto& pObj = m_aObjects[m_nCount];
        ++m_nCount;
        return pObj;
    }
};



#endif //LIVERUSSIA_CSTORE_H

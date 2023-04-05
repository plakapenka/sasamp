//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_LINK_H
#define LIVERUSSIA_LINK_H


template <typename T>
class CLink {
public:
    T         data;
    CLink<T>* prev;
    CLink<T>* next;

    void Remove() {
        next->prev = prev;
        prev->next = next;
    }

    void Insert(CLink<T>* link) {
        link->next = next;
        next->prev = link;
        link->prev = this;
        next = link;
    }
};



#endif //LIVERUSSIA_LINK_H

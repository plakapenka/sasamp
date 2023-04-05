//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLLISION_H
#define LIVERUSSIA_COLLISION_H


#include "CollisionData.h"
#include "../Core/LinkList.h"

class CCollision {
public:
    static inline CLinkList<CCollisionData*> ms_colModelCache{};
    static inline uint32_t ms_iProcessLineNumCrossings = 0;
    static inline uint32_t ms_collisionInMemory = 0;

public:
    static void InjectHooks();
};


#endif //LIVERUSSIA_COLLISION_H

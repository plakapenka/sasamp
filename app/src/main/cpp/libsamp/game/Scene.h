//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_SCENE_H
#define LIVERUSSIA_SCENE_H


#include "RW/rpworld.h"
#include "RW/rwcore.h"

class CScene {
public:
    RpWorld*  m_pRpWorld;
    RwCamera* m_pRwCamera;
};

extern CScene Scene;

#endif //LIVERUSSIA_SCENE_H

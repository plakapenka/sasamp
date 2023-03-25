//
// Created by plaka on 24.03.2023.
//

#ifndef LIVERUSSIA_CAMERAGTA_H
#define LIVERUSSIA_CAMERAGTA_H


#include "CPlaceableGta.h"

class CameraGta : CPlaceableGta{
    bool            m_bAboveGroundTrainNodesLoaded{};
    bool            m_bBelowGroundTrainNodesLoaded{};
    bool            m_bCamDirectlyBehind{};
    bool            m_bCamDirectlyInFront{};
    bool            m_bCameraJustRestored{};
    bool            m_bCutsceneFinished{};
    bool            m_bCullZoneChecksOn{};
};


#endif //LIVERUSSIA_CAMERAGTA_H

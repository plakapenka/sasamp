//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_QUEUEDMODE_H
#define LIVERUSSIA_QUEUEDMODE_H


class CQueuedMode {
public:
    uint16_t    m_nMode;
    uint8_t     skip1;
    uint8_t     skip2;
    float       m_fDuration;
    uint16_t    m_nMinZoom;
    uint16_t    m_nMaxZoom;
};
static_assert(sizeof(CQueuedMode) == 0xC, "Invalid");

#endif //LIVERUSSIA_QUEUEDMODE_H

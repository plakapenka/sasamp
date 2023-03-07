//
// Created by plaka on 24.02.2023.
//

#ifndef LIVERUSSIA_CTRANSMISSION_H
#define LIVERUSSIA_CTRANSMISSION_H

struct tTransmissionGear
{
    float m_fMaxVelocity;
    float m_fChangeUpVelocity;
    float m_fChangeDownVelocity;
};

class cTransmission {
public:
    tTransmissionGear m_aGears[6];
    int8_t m_nDriveType; // F/R/4
    int8_t m_nEngineType; // P/D/E
    int8_t m_nNumberOfGears; // 1 to 6
    int8_t field_4B;
    uint32_t m_nHandlingFlags;
    float         m_fEngineAcceleration; // 0.1 to 10.0
    float         m_fEngineInertia; // 0.0 to 50.0
    float         m_fMaxGearVelocity; // 5.0 to 150.0
    float         m_fMaxFlatVelocity;
    float         m_fMaxReverseVelocity;
    float         m_fCurrentSpeed;
};


#endif //LIVERUSSIA_CTRANSMISSION_H

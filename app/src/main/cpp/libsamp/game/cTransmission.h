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
    unsigned char m_nDriveType; // F/R/4
    unsigned char m_nEngineType; // P/D/E
    unsigned char m_nNumberOfGears; // 1 to 6
    char field_4B;
    unsigned int  m_nHandlingFlags;
    float         m_fEngineAcceleration; // 0.1 to 10.0
    float         m_fEngineInertia; // 0.0 to 50.0
    float         m_fMaxGearVelocity; // 5.0 to 150.0
    int field_5C;
    float         m_fMinGearVelocity;
    float         m_fCurrentSpeed;
};


#endif //LIVERUSSIA_CTRANSMISSION_H

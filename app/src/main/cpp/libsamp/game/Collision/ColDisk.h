//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COLDISK_H
#define LIVERUSSIA_COLDISK_H

class CVector;

struct CColDisk : CColSphere
{
    CVector m_vecThickness;
    float m_fThickness;
};


#endif //LIVERUSSIA_COLDISK_H

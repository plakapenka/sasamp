//
// Created by plaka on 29.03.2023.
//

#ifndef LIVERUSSIA_COLLISIONDATA_H
#define LIVERUSSIA_COLLISIONDATA_H


#include "ColSphere.h"
#include "ColLine.h"
#include "ColBox.h"
#include "ColTriangle.h"
#include "ColTrianglePlane.h"
#include "ColDisk.h"
#include "../Core/Link.h"

class CCollisionData {
public:
    CCollisionData();

public:
    uint16_t m_nNoOfSpheres;
    uint16_t m_nNoOfBoxes;
    uint16_t m_nNoOfTriangles;
    union {
        uint8_t     m_nNoOfLines;
        uint8_t     m_nNoOfDisks;
    };
    struct {
        uint8_t m_useDisksNotLines : 1;     // 0x1 - Always set to false
        uint8_t m_useColModelSections : 1; // 0x2 - See the huge comment below
        uint8_t m_hasShadowTris : 1; // 0x4 - See wiki.
    };
    CColSphere* m_pSphereArray;
    CColBox* m_pBoxes;
    union {
        CColLine *m_pLineArray;
        CColDisk *m_pDiskArray;
    };

    CVector* m_pTriCompressedVectorArray;

    CColTriangle*      m_pTriangleArray;            // 0x18
    CColTrianglePlane*      m_pTrianglePlanes;       // 0x1C
    int32_t         m_nNoOfShadTriangles;   // 0x20
    int32_t         m_nNoOfShadTriangleVerts;    // 0x24
    CVector*        m_pTriCompressedShadVectorArray;       // 0x28
    CColTriangle*   m_pShadTriangleArray;      // 0x2C
    int32_t*        m_modelSec;

public:
    CLink<CCollisionData*>* GetLinkPtr();
};
static_assert(sizeof(CCollisionData) == 0x34, "Invalid size CCollisionData");


#endif //LIVERUSSIA_COLLISIONDATA_H

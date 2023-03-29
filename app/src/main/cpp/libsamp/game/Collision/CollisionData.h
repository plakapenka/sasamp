//
// Created by plaka on 29.03.2023.
//

#ifndef LIVERUSSIA_COLLISIONDATA_H
#define LIVERUSSIA_COLLISIONDATA_H


#include "ColSphere.h"
#include "ColLine.h"

struct CCollisionData {
    uint16_t m_nNoOfSpheres;
    uint16_t m_nNoOfBoxes;
    uint16_t m_nNoOfTriangles;
    union {
        uint8_t     m_nNoOfLines;
        uint8_t     m_nNoOfDisks;
    };
    struct {
        uint8_t bUsesDisks : 1;     // 0x1 - Always set to false
        uint8_t bHasFaceGroups : 1; // 0x2 - See the huge comment below
        uint8_t bHasShadowInfo : 1; // 0x4 - See wiki.
    };
    CColSphere* m_pSphereArray;
    uintptr_t* m_pBoxes;
    union {
        CColLine* m_pLines;
        uintptr_t* m_pDisks;
    };

    uintptr_t* m_pVertices;

    // If you take a look here: https://gtamods.com/wiki/Collision_File#Body
    // You may notice there's an extra section called `TFaceGroups` before `TFace` (triangles)
    // And it is used in `CCollision::ProcessColModels`.
    //
    // The following is only true if `bHasFaceGroups` flag is set (Which is the case only when loaded by `CFileLoader::LoadCollisionModelVer2/3/4`). **
    // All the data is basically stored in a big buffer, and all the data pointers - except `m_pTrianglePlanes` - point into it.
    // In case the flag `bHasFaceGroups` is set there's an array of `TFaceGroup` before the triangles, but there's no pointer to it.
    // In order to access it you have to do some black magic with `pTriangles`. Here's the memory layout of the `TFaceGroups` data:
    //
    // FaceGroup[]          - -0x8 - And growing downwards by `sizeof(FaceGroup)` (Which is 28)
    // uint32 nFaceGroups;  - -0x4
    // <Triangles>          - FaceGroup data is before the triangles!
    //
    // Whenever accessing this section make sure both `CColModel::bSingleAlloc` and `bHasFaceGroups` is set
    // (also, please, assert if `bHasFaceGroups` is set but `bSingleAlloc` isnt)
    //
    // NOTEs:
    // ** Col models may also be loaded by the Collision plugin from a clump file - In this case `CFileLoader::LoadCollisionModelVer2/3` is called, but then
    //    the col data is reallocated using `MakeMultipleAlloc` which uses `Copy` to copy the data, in this case the face groups aren't copied. (And the flag is set to false in the ctor)
    uintptr_t*      m_pTriangles;            // 0x18
    uintptr_t*      m_pTrianglePlanes;       // 0x1C
    uint32_t        m_nNumShadowTriangles;   // 0x20
    uint32_t        m_nNumShadowVertices;    // 0x24
    uintptr_t*      m_pShadowVertices;       // 0x28
    uintptr_t*      m_pShadowTriangles;      // 0x2C
    uint32_t        m_modelSec;
};
static_assert(sizeof(CCollisionData) == 0x34, "Invalid size CCollisionData");


#endif //LIVERUSSIA_COLLISIONDATA_H

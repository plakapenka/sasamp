//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_VEHICLEMODELINFO_H
#define LIVERUSSIA_VEHICLEMODELINFO_H


#include "ClumpModelInfo.h"
#include "../RW/rwlpcore.h"
#include "../Enums/VehicleClass.h"
#include "../Core/Vector.h"
#include "../Core/Quaternion.h"
#include "game/Enums/VehicleType.h"

union tVehicleCompsUnion {
    unsigned int m_nComps;
    struct {
        unsigned int nExtraA_comp1 : 4;
        unsigned int nExtraA_comp2 : 4;
        unsigned int nExtraA_comp3 : 4;
        unsigned int : 4;

        unsigned int nExtraB_comp1 : 4;
        unsigned int nExtraB_comp2 : 4;
        unsigned int nExtraB_comp3 : 4;
        unsigned int : 4;
    };
    struct {
        unsigned int nExtraAComp : 12;
        unsigned int nExtraARule : 4;

        unsigned int nExtraBComp : 12;
        unsigned int nExtraBRule : 4;
    };

    struct {
        unsigned int nExtraA : 16;
        unsigned int nExtraB : 16;
    };
};

struct UpgradePosnDesc {
public:
    UpgradePosnDesc(){};
    ~UpgradePosnDesc(){};

public:
    CVector m_vPosition;
    CQuaternion m_qRotation;
    int m_nParentComponentId;
};

struct CVehicleModelInfo : public CClumpModelInfo {
    RpMaterial*        m_pPlateMaterial; // 3C
    char               m_szPlateText[9]; // 40
    uint8_t            m_nPlateType;    //  49
    char               m_szGameName[8]; //  4a
    char               _pad3A[2];       //  52
    eVehicleType       m_nVehicleType;  //  54
    float              m_fWheelSizeFront;// 58
    float              m_fWheelSizeRear;    //5C
    int16_t            m_nWheelModelIndex;  //60
    int16_t            m_nHandlingId;       // 62
    // int8_t             field_4B;
    uint8_t            m_nNumDoors;     //64
    eVehicleClass      m_nVehicleClass; //65
    uint8_t            m_nFlags;    //66
    unsigned char      m_nWheelUpgradeClass;//67
    unsigned char      m_nTimesUsed;    //68
    char               field_51;    // 69
    unsigned short     m_nFrq;      // 6A
    tVehicleCompsUnion m_extraComps;    //6c
    float              m_fBikeSteerAngle; // 70

    struct {
        CVector         m_avDummyPos[15];
        UpgradePosnDesc m_aUpgrades[18];
        RpAtomic*       m_apExtras[6];
        uint8_t         m_nNumExtras;
        uint32_t        m_nMaskComponentsDamagable;
    } * m_pVehicleStruct;   // 74

    uint32_t m_firstColour[49];
    uint32_t m_secondColour[33];
    uint32_t m_thirdColour[17];
    uint32_t m_fourthColour[17];
    uintptr_t* m_apDirtMaterials[64];
    uint8_t m_possibleColours[32];
    uint8_t m_numPossibleColours;
    uint8_t m_lastColUsed;
    uintptr_t *m_lastCol;
    int16_t   m_anUpgrades[18];
    uint8_t skip_0[2];
    uint32_t m_anRemapTxds[4];

    union {
        uintptr_t* m_pAnimBlock;
        char* m_animBlockFileName;
        unsigned int m_dwAnimBlockIndex;
    };
};


#endif //LIVERUSSIA_VEHICLEMODELINFO_H

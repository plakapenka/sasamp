//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_CVEHICLEMODELINFO_H
#define LIVERUSSIA_CVEHICLEMODELINFO_H


#include "Models/CClumpModelInfo.h"
#include "game/RW/rwlpcore.h"
#include "game/Enums/eVehicleClass.h"
#include "CVector.h"
#include "quaternion.h"


enum eVehicleType {
    VEHICLE_IGNORE = -1,
    VEHICLE_AUTOMOBILE = 0,
    VEHICLE_MTRUCK, // MONSTER TRUCK
    VEHICLE_QUAD,
    VEHICLE_HELI,
    VEHICLE_PLANE,
    VEHICLE_BOAT,
    VEHICLE_TRAIN,
    VEHICLE_FHELI,
    VEHICLE_FPLANE,
    VEHICLE_BIKE,
    VEHICLE_BMX,
    VEHICLE_TRAILER
};

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
    unsigned char      m_nPlateType;    //  49
    char               m_szGameName[8]; //  4a
    char               _pad3A[2];       //  52
    unsigned int       m_nVehicleType;  //  54
    float              m_fWheelSizeFront;// 58
    float              m_fWheelSizeRear;    //5C
    short              m_nWheelModelIndex;  //60
    short              m_nHandlingId;       // 62
    // int8_t             field_4B;
    unsigned char      m_nNumDoors;     //64
    eVehicleClass      m_nVehicleClass; //65
    unsigned char      m_nFlags;    //66
    unsigned char      m_nWheelUpgradeClass;//67
    unsigned char      m_nTimesUsed;    //68
    char               field_51;    // 69
    unsigned short     m_nFrq;      // 6A
    tVehicleCompsUnion m_extraComps;    //6c
    float              m_fBikeSteerAngle; // 70

    struct {
        CVector m_avDummyPos[15];
        UpgradePosnDesc m_aUpgrades[18];
        RpAtomic* m_apExtras[6];
        unsigned char m_nNumExtras;
        unsigned int m_nMaskComponentsDamagable;
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


#endif //LIVERUSSIA_CVEHICLEMODELINFO_H

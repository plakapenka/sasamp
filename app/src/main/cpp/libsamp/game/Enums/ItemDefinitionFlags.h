//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_ITEMDEFINITIONFLAGS_H
#define LIVERUSSIA_ITEMDEFINITIONFLAGS_H


// https://gtamods.com/wiki/Item_Definition#IDE_Flags
enum eItemDefinitionFlags {
    IS_ROAD                     = 0x1,
    PAD1_                       = 0x2,
    DRAW_LAST                   = 0x4,
    ADDITIVE                    = 0x8,
    PAD2_                       = 0x10,
    ANIM_SOMETHING              = 0x20,
    NO_ZBUFFER_WRITE            = 0x40,
    DONT_RECEIVE_SHADOWS        = 0x80,
    PAD3                        = 0x100,
    IS_GLASS_TYPE_1             = 0x200,
    IS_GLASS_TYPE_2             = 0x400,
    IS_GARAGE_DOOR              = 0x800,
    IS_DAMAGEABLE               = 0x1000,
    IS_TREE                     = 0x2000,
    IS_PALM                     = 0x4000,
    DOES_NOT_COLLIDE_WITH_FLYER = 0x8000,
    IS_EXPLOSIVE_UNUSED         = 0x10000,
    IS_COP_BODYPART_UNUSED      = 0x20000,
    PAD6                        = 0x40000,
    IS_UNKNOWN                  = 0x80000,
    IS_TAG                      = 0x100000,
    DISABLE_BACKFACE_CULLING    = 0x200000,
    IS_BREAKABLE_STATUE         = 0x400000
};

struct sItemDefinitionFlags {
    union {
        uint32_t dwFlags;
        struct {
            uint32_t bIsRoad             : 1;
            uint32_t bPad1               : 1;
            uint32_t bDrawLast           : 1;
            uint32_t bAdditive           : 1;
            uint32_t bPad2               : 1;
            uint32_t bAnimSomething      : 1;
            uint32_t bNoZBufferWrite     : 1;
            uint32_t bDontReceiveShadows : 1;

            uint32_t bPad3                    : 1;
            uint32_t bIsGlassType1            : 1;
            uint32_t bIsGlassType2            : 1;
            uint32_t bIsGarageDoor            : 1;
            uint32_t bIsDamageable            : 1;
            uint32_t bIsTree                  : 1;
            uint32_t bIsPalm                  : 1;
            uint32_t bDoesNotCollideWithFlyer : 1;

            uint32_t bIsExplosive_Unused     : 1;
            uint32_t bIsCopBodyPart_Unused   : 1;
            uint32_t bPad4                   : 1;
            uint32_t bIsUnknown              : 1;
            uint32_t bIsTag                  : 1;
            uint32_t bDisableBackfaceCulling : 1;
            uint32_t bisBreakableStatue      : 1;
        };
    };

    inline sItemDefinitionFlags(uint32_t flags) : dwFlags(flags) {}
};

#endif //LIVERUSSIA_ITEMDEFINITIONFLAGS_H

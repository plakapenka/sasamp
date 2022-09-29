#pragma once

class CSprite2d;

enum eHudSprites {
    HUDSPRITE_FIST,
    HUDSPRITE_SITEM16,
    HUDSPRITE_SITEROCKET,
    HUDSPRITE_RADARDISC,
    HUDSPRITE_RADARRINGPLANE,
    HUDSPRITE_SKIPICON
};

class CHud
{
private:
    /* data */
public:
    static CSprite2d **Sprites; // static CSprite2d Sprites[6]
};

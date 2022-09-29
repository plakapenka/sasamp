#pragma once
#include "../game/game.h"

class CCrossHair : private CSprite2d
{
private:
    bool m_bNeedFix;
public:
    CCrossHair(/* args */);
    ~CCrossHair();

    void Render();

    bool IsNeededFix() { return m_bNeedFix; }
};



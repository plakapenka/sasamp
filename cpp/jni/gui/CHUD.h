#pragma once

enum HUD_SELECT
{   
    CHUD_NOP,
    CHUD_ONE,
    CHUD_TWO
};

class CHUD
{
public:
    static bool m_bShow;
    static HUD_SELECT m_iHud;
    static ImFont* speedFont;

    // textures
    static RwTexture* hud_1_background, *hud_1_fist, *hud_1_star;
    static RwTexture* hud_2_radar_background, *hud_2_star_background, *hud_2_star, *hud_2_cashicon, *hud_2_health, *hud_2_armour, *hud_2_eat;

    static RwTexture* speed_1_fuel, *speed_1_health;
    static RwTexture* speed_1_engine[2], *speed_1_light[2], *speed_1_lock[2];

    static uint16_t iHungry, iVehFuel, iVehLight, iVehEngine, iVehLock;
public:
    static void Initialise();
    static void Select(HUD_SELECT hudNum);

    static void Disable()      { m_bShow = false; }
    static void Enable()       { m_bShow = true; };
    static bool IsEnabled()    { return m_bShow; };

    static void EditFist(CRect* rect);
    static void EditRadar(CRect* rect);
    static void Render();
};

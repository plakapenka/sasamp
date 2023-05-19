#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "../util/CJavaWrapper.h"
#include "java_systems/CHUD.h"
#include "gui/gui.h"

extern CGame *pGame;
extern CGUI* pGUI;

RwTexture* splashTexture = nullptr;

#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_BLACK 	0xFF000000
#define COLOR_ORANGE 	0xFFFAA500
#define COLOR_PURPLE	0xFFCE39DF
#define COLOR_ROSE		0xFFFF99FF
#define COLOR_GREY		0xFF5d5d5d
#define COLOR_BRED		0xFF9933FF
#define COLOR_BLUE		0xFF6C2713
#define COLOR_CYAN		0xFFCE6816
#define COLOR_1			0xFFB58891
#define COLOR_2			0xFF673F40
#define COLOR_PIZDEC	0xFFff008b

struct stRect
{
	int x1;	// left
	int y1;	// top
	int x2;	// right
	int y2;	// bottom
};

struct stfRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

#define MAX_SCHEMAS 4
uint32_t colors[MAX_SCHEMAS][2] = {
	{ COLOR_BLACK,	COLOR_WHITE },
	{ COLOR_BLACK, 	COLOR_ORANGE },
	{ COLOR_BLACK,	COLOR_PIZDEC },
	{ COLOR_BLACK,	COLOR_BLUE }
};
unsigned int color_scheme = 0;

void RenderBackgroundHud()
{
	if (CHUD::bIsShow)
	{

		if (splashTexture)
		{//183 55.. 399..33
			ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)splashTexture->raster,
													 ImVec2(CHUD::radarBgPos1.x, CHUD::radarBgPos1.y),
													 ImVec2(CHUD::radarBgPos2.x, CHUD::radarBgPos2.y),
													 ImVec2(0, 0), ImVec2(1, 1));
		}
	}

}
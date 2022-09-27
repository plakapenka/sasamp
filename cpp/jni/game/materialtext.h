#include "gui/gui.h"

#define OBJECT_MATERIAL_TEXT_ALIGN_LEFT		0
#define OBJECT_MATERIAL_TEXT_ALIGN_CENTER	1
#define OBJECT_MATERIAL_TEXT_ALIGN_RIGHT	2

class CMaterialText
{
public:
    CMaterialText();

    RwTexture* Generate(uint8_t matSize, const char* fontname, float fontSize, uint8_t bold, uint32_t fontcol, uint32_t backcol, uint8_t align, const char* szText);
    RwTexture* GenerateNumberPlate(const char* szPlate) { return Generate(70, "", 40, 0, 0xffff6759, 0x0, OBJECT_MATERIAL_TEXT_ALIGN_CENTER, szPlate); }
private:
    void SetUpScene();
    void SetUpMaterial(uint8_t matSize, const char* fontname, float fontSize, uint8_t bold, ImColor fontcol, ImColor backcol, uint8_t align, const char* szText);

    RwCamera* m_camera;
    RwFrame* m_zBuffer;
};

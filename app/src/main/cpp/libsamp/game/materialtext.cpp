#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
#include "materialtext.h"
#include "common.h"

extern CGame *pGame;
extern CGUI *pGUI;

/* imgui_impl_renderware.h */
bool ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_CreateDeviceObjects();

CMaterialText::CMaterialText()
{
    m_camera = 0;
    m_zBuffer = 0;
    SetUpScene();
}

void CMaterialText::SetUpScene()
{
    m_camera = RwCameraCreate();
    m_zBuffer = RwFrameCreate();

    RwObjectHasFrameSetFrame(m_camera, m_zBuffer);
    RwCameraSetFarClipPlane(m_camera, 300.0f);
    RwCameraSetNearClipPlane(m_camera, 0.01f);

    RwV2d view = { 0.5f, 0.5f };
    RwCameraSetViewWindow(m_camera, view);
    RwCameraSetProjection(m_camera, rwPERSPECTIVE);
    RpWorld* pRwWorld = *(RpWorld**)(g_libGTASA + 0x95B060);
    if (pRwWorld) {
        RpWorldAddCamera(pRwWorld, m_camera);
    }
}

static uint16_t sizes[14][2] = {
            { 32 * 2 , 32 * 2 } , { 64 * 2, 32 * 2}, { 64 * 2, 64 * 2 }, { 128 * 2, 32 * 2}, { 128 * 2, 64 * 2}, { 128 * 2,128 * 2}, { 256 * 2, 32 * 2},
            { 256 * 2, 64 * 2} , { 256 * 2, 128 * 2} , { 256 * 2, 256 * 2} , { 512 * 2, 64 * 2} , { 512 * 2,128 * 2 } , { 512 * 2,256 * 2} , { 512 * 2,512  * 2}
    };

RwTexture* CMaterialText::Generate(uint8_t matSize, const char* fontname, float fontSize, uint8_t bold, uint32_t fontcol, uint32_t backcol, uint8_t align, const char* szText)
{

    matSize = (matSize / 10) - 1;

    RwRaster* raster = RwRasterCreate(sizes[matSize][0] * 2, sizes[matSize][1] * 2, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
    RwTexture* bufferTexture = RwTextureCreate(raster);
    if(!raster || !bufferTexture) return 0;

    // set camera frame buffer
    m_camera->frameBuffer = raster;

    //util::game::SetRenderWareCamera(m_camera);
    ((void(*)(RwCamera *))(g_libGTASA + 0x55CFA4 + 1))(m_camera);

    RwRGBA* rgba = (RwRGBA*)&backcol;
    RwUInt8 r = rgba->blue;
    RwUInt8 b = rgba->red;

    rgba->red = r;
    rgba->blue = b;
    Log("MaterialText: backcol - 0x%X 0x%X 0x%X 0x%X", rgba->red, rgba->green, rgba->blue, rgba->alpha);

    RwCameraClear(m_camera, (RwRGBA*)&backcol, 3);
    RwCameraBeginUpdate(m_camera);

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)rwCULLMODENACULLMODE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

    ((void(*)())(g_libGTASA + 0x559008 + 1))();

    Log("MaterialText: backcol - 0x%X", backcol);

    /*int bgB = (backcol) & 0xFF;
    int bgG = (backcol >> 8) & 0xFF;
    int bgR = (backcol >> 16) & 0xFF;
    int bgA = (backcol >> 24) & 0xFF;*/

    


    //Log("MaterialText: backcol - 0x%X%X%X%X", bgR, bgG, bgB, bgA);

    int fontB = (fontcol) & 0xFF;
    int fontG = (fontcol >> 8) & 0xFF;
    int fontR = (fontcol >> 16) & 0xFF;
    int fontA = (fontcol >> 24) & 0xFF;

    SetUpMaterial(matSize, fontname, fontSize * 2.0f, bold, ImColor(fontR, fontG, fontB, fontA), ImColor(backcol)/*ImColor(bgR, bgG, bgB, bgA)*/, align, szText);

    RwCameraEndUpdate(m_camera);
    return bufferTexture;
}

void CMaterialText::SetUpMaterial(uint8_t matSize, const char* fontname, float fontSize, uint8_t bold, ImColor fontcol, ImColor backcol, uint8_t align, const char* szText)
{
    ImGui::NewFrame();

    char utf8[2048];
    cp1251_to_utf8(utf8, szText, 2048);

    // text align
    ImVec2 vecPos;

    std::string strText = utf8;
    std::stringstream ssLine(strText);
    std::string tmpLine;
    int newLineCount = 0;

    Log("TEXT: %s", utf8);

    switch(align)
    {
        case OBJECT_MATERIAL_TEXT_ALIGN_LEFT:
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(tmpLine[0] != 0)
                {
                    vecPos.x = 0;
                    vecPos.y = (sizes[matSize][1] - fontSize) / 2;
                }
            }
            break;
        case OBJECT_MATERIAL_TEXT_ALIGN_CENTER:
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(tmpLine[0] != 0)
                {
                    vecPos.x = (sizes[matSize][0] / 2) - (UIUtils::CalcTextSizeWithoutTags((char *)tmpLine.c_str(), fontSize).x / 2);
                    vecPos.y = (sizes[matSize][1] / 2) - (UIUtils::CalcTextSizeWithoutTags((char *)tmpLine.c_str(), fontSize).y / 2);
                }
            }
            break;
        case OBJECT_MATERIAL_TEXT_ALIGN_RIGHT:
            while(std::getline(ssLine, tmpLine, '\n'))
            {
                if(tmpLine[0] != 0)
                {
                    vecPos.x = (sizes[matSize][0] - UIUtils::CalcTextSizeWithoutTags((char *)tmpLine.c_str(), fontSize).x);
                    vecPos.y = (sizes[matSize][1] - fontSize);
                }
            }
            break;
    }

    if(backcol)
    {
        ImVec2 backpos = ImVec2(vecPos.x-pGUI->GetFontSize(), vecPos.y-pGUI->GetFontSize());
        ImVec2 backsize = ImVec2(fontSize+pGUI->GetFontSize(), fontSize+pGUI->GetFontSize());
        ImGui::GetBackgroundDrawList()->AddRectFilled(backpos, backsize, backcol);
    }
    vecPos.x *= 2;
    vecPos.y *= 2;
    UIUtils::drawText(vecPos, fontcol, utf8, fontSize * 2);

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}
#include "UIUtils.h"
#include "gui.h"

extern CGUI *pGUI;

bool UIUtils::ProcessInlineHexColor(const char* start, const char* end, ImVec4& color)
{
    const int hexCount = (int)(end-start);
    if(hexCount == 6 || hexCount == 8)
    {
        char hex[9];
        strncpy(hex, start, hexCount);
        hex[hexCount] = 0;

        unsigned int hexColor = 0;
        if(sscanf(hex, "%x", &hexColor)	> 0)
        {
            color.x = static_cast< float >((hexColor & 0x00FF0000) >> 16) / 255.0f;
            color.y = static_cast< float >((hexColor & 0x0000FF00) >> 8) / 255.0f;
            color.z = static_cast< float >((hexColor & 0x000000FF)) / 255.0f;
            color.w = 1.0f;

            if(hexCount == 8)
                color.w = static_cast< float >((hexColor & 0xFF000000) >> 24) / 255.0f;

            return true;
        }
    }

    return false;
}

void UIUtils::drawText(const char* fmt, ...)
{
    char tempStr[4096];

    va_list argPtr;
    va_start(argPtr, fmt);
    vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
    va_end(argPtr);
    tempStr[sizeof(tempStr) - 1] = '\0';

    bool pushedColorStyle = false;
    const char* textStart = tempStr;
    const char* textCur = tempStr;
    while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
    {
        if (*textCur == '{')
        {
            // Print accumulated text
            if (textCur != textStart)
            {
                ImGui::TextUnformatted(textStart, textCur);
                ImGui::SameLine(0.0f, 0.0f);
            }

            // Process color code
            const char* colorStart = textCur + 1;
            do
            {
                ++textCur;
            } while (*textCur != '\0' && *textCur != '}');

            // Change color
            if (pushedColorStyle)
            {
                ImGui::PopStyleColor();
                pushedColorStyle = false;
            }

            ImVec4 textColor;
            if (ProcessInlineHexColor(colorStart, textCur, textColor))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                pushedColorStyle = true;
            }

            textStart = textCur + 1;
        }
        else if (*textCur == '\n')
        {
            // Print accumulated text an go to next line
            ImGui::TextUnformatted(textStart, textCur);
            textStart = textCur + 1;
        }

        ++textCur;
    }

    if (textCur != textStart)
        ImGui::TextUnformatted(textStart, textCur);
    else
        ImGui::NewLine();

    if(pushedColorStyle)
        ImGui::PopStyleColor();
}

void UIUtils::drawText(ImVec2 pos, ImColor col, const char* szStr, const float font_size)
{
    char tempStr[4096];

    //va_list argPtr;
    //va_start(argPtr, fmt);
    //vsnprintf(tempStr, sizeof(tempStr), fmt, argPtr);
    //va_end(argPtr);

    ImVec2 vecPos = pos;

    strcpy(tempStr, szStr);
    tempStr[sizeof(tempStr) - 1] = '\0';

    bool pushedColorStyle = false;
    const char* textStart = tempStr;
    const char* textCur = tempStr;
    while(textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
    {
        if (*textCur == '{')
        {
            // Print accumulated text
            if (textCur != textStart)
            {
                pGUI->RenderTextWithSize(vecPos, col, true, textStart, textCur, font_size);
                vecPos.x += pGUI->GetFont()->CalcTextSizeA(font_size, FLT_MAX, -1.0f, textStart, textCur).x;
            }

            // Process color code
            const char* colorStart = textCur + 1;
            do
            {
                ++textCur;
            } while (*textCur != '\0' && *textCur != '}');

            // Change color
            if (pushedColorStyle)
                pushedColorStyle = false;

            ImVec4 textColor;
            if (UIUtils::ProcessInlineHexColor(colorStart, textCur, textColor))
            {
                col = textColor;
                pushedColorStyle = true;
            }

            textStart = textCur + 1;
        }
        else if (*textCur == '\n')
        {
            pGUI->RenderTextWithSize(vecPos, col, true, textStart, textCur, font_size);
            vecPos.x = pos.x;
            vecPos.y += font_size;
            textStart = textCur + 1;
        }

        ++textCur;
    }

    if (textCur != textStart)
    {
        pGUI->RenderTextWithSize(vecPos, col, true, textStart, textCur, font_size);
        vecPos.x += pGUI->GetFont()->CalcTextSizeA(font_size, FLT_MAX, -1.0f, textStart, textCur).x;
    }
    else
        vecPos.y += font_size;
}

void UIUtils::drawText(ImVec2 pos, char* utf8string, uint32_t dwColor)
{
    uint16_t linesCount = 0;
    std::string strUtf8 = utf8string;
    int size = strUtf8.length();
    std::string color;

    for(uint32_t i = 0; i < size; i++)
    {
        if(i+7 < strUtf8.length())
        {
            if(strUtf8[i] == '{' && strUtf8[i+7] == '}' )
            {
                color = strUtf8.substr(i, 7+1);
            }
        }
        if(strUtf8[i] == '\n')
        {
            linesCount++;
            if(i+1 < strUtf8.length() && !color.empty())
            {
                strUtf8.insert(i+1 , color);
                size += color.length();
                color.clear();
            }
        }
        if(strUtf8[i] == '\t')
        {
            strUtf8.replace(i, 1, " ");
        }
    }
    pos.y += pGUI->GetFontSize()*(linesCount / 2);
    if(linesCount)
    {
        uint16_t curLine = 0;
        uint16_t curIt = 0;
        for(uint32_t i = 0; i < strUtf8.length(); i++)
        {
            if(strUtf8[i] == '\n')
            {
                if(strUtf8[curIt] == '\n' )
                {
                    curIt++;
                }
                ImVec2 _pos = pos;
                _pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, i-curIt).c_str()).x / 2;
                _pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
                drawText(_pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, i-curIt).c_str(), pGUI->GetFontSize());
                curIt = i;
                curLine++;
            }
        }
        if(strUtf8[curIt] == '\n')
        {
            curIt++;
        }
        if(strUtf8[curIt] != '\0')
        {
            ImVec2 _pos = pos;
            _pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.substr(curIt, strUtf8.size()-curIt).c_str()).x / 2;
            _pos.y -= ( pGUI->GetFontSize()*(linesCount - curLine) );
            drawText(_pos, __builtin_bswap32(dwColor), (char*)strUtf8.substr(curIt, strUtf8.length()-curIt).c_str(), pGUI->GetFontSize());
        }
    }
    else
    {
        pos.x -= CalcTextSizeWithoutTags((char*)strUtf8.c_str()).x / 2;
        drawText(pos, __builtin_bswap32(dwColor), (char*)strUtf8.c_str(), pGUI->GetFontSize());
    }
}

void UIUtils::FilterColors(char* szStr)
{
    if(!szStr) return;

    char szNonColored[2048+1];
    int iNonColoredMsgLen = 0;

    for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
    {
        if(pos+7 < strlen(szStr))
        {
            if(szStr[pos] == '{' && szStr[pos+7] == '}')
            {
                pos += 7;
                continue;
            }
        }

        szNonColored[iNonColoredMsgLen] = szStr[pos];
        iNonColoredMsgLen++;
    }

    szNonColored[iNonColoredMsgLen] = 0;
    strcpy(szStr, szNonColored);
}

ImVec2 UIUtils::CalcTextSizeWithoutTags(char* szStr, float font_size)
{
    if(!szStr) return ImVec2(0, 0);

    char szNonColored[2048+1];
    int iNonColoredMsgLen = 0;

    for(int pos = 0; pos < strlen(szStr) && szStr[pos] != '\0'; pos++)
    {
        if(pos+7 < strlen(szStr))
        {
            if(szStr[pos] == '{' && szStr[pos+7] == '}')
            {
                pos += 7;
                continue;
            }
        }

        szNonColored[iNonColoredMsgLen] = szStr[pos];
        iNonColoredMsgLen++;
    }

    szNonColored[iNonColoredMsgLen] = 0;

    ImVec2 text_size;

    if(font_size)
    {
        text_size = pGUI->GetFont()->CalcTextSizeA(font_size, FLT_MAX, -1.0f, szNonColored);
        text_size.x = (float)(int)(text_size.x + 0.95f);
    }
    else
        text_size = ImGui::CalcTextSize(szNonColored);

    return text_size;
}

std::string UIUtils::removeColorTags(std::string line)
{
    std::string string;
    for(uint32_t it = 0; it < line.size(); it++)
    {
        if(it+7 < line.size())
        {
            if(line[it] == '{' && line[it+7] == '}')
            {
                it += 7;
                continue;
            }
        }
        string.push_back(line[it]);
    }
    return string;
}

void UIUtils::CreateCircleProgressBar(ImGuiWindow* window, ImVec2 pos, ImColor color, int value, float radius, float thickness)
{
    float _value = radius / 10 / (float)value;

    const float a0 = 1.3 / 6.0f * 2.0f * IM_PI;
    const float a1 = _value / 6.0f * 2.0f * IM_PI;

    window->DrawList->PathClear();
    window->DrawList->PathArcTo(pos, radius, a0, a1, 50);
    window->DrawList->PathStroke(color, false, ImMax(2.0f, thickness));
}
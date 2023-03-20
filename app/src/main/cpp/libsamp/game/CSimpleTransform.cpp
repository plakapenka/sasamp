//
// Created by plaka on 20.03.2023.
//

#include <cstdint>
#include "CSimpleTransform.h"

extern uintptr_t g_libGTASA;

void CSimpleTransform::UpdateRwMatrix(RwMatrix* out)
{
    ((void(*)(CSimpleTransform*, RwMatrix*))(g_libGTASA + 0x00408618 + 1))(this, out);
}

void CSimpleTransform::Invert(const CSimpleTransform& base)
{
    ((void(*)(CSimpleTransform*, const CSimpleTransform&))(g_libGTASA + 0x0040869A + 1))(this, base);
}

void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
    ((void(*)(CSimpleTransform*, class CMatrix*))(g_libGTASA + 0x0040867C + 1))(this, out);
}

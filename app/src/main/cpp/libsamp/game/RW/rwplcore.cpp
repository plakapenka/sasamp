//
// Created by plaka on 22.03.2023.
//


#include "rwlpcore.h"
#include "../../main.h"

RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    return ((RwBool(__cdecl *)(RwMatrix*))g_libGTASA + 0x001E446C + 1)(mpMat);
}
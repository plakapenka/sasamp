//
// Created by plaka on 23.03.2023.
//

#include "rwlpcore.h"
#include "../../main.h"

RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    matrix->flags &= 0xFFFDFFFC;
    return matrix;
}

RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    return ((RwBool(__cdecl *)(RwMatrix*))(g_libGTASA + 0x001E446C + 1))(mpMat);
}
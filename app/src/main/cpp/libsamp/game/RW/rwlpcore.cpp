//
// Created by plaka on 23.03.2023.
//

#include "rwlpcore.h"

RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    matrix->flags &= 0xFFFDFFFC;
    return matrix;
}
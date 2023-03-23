//
// Created by plaka on 23.03.2023.
//

#include "CMatrix.h"

//
//uint8_t* CMatrix::EulerIndices1 = (uint8_t*)0x866D9C;
//uint8_t* CMatrix::EulerIndices2 = (uint8_t*)0x866D94;
//int32_t& numMatrices = *(int32_t*)0xB74238;
//CMatrix& gDummyMatrix = *(CMatrix*)0xB74240;


CMatrix::CMatrix(const CMatrix& matrix) {
    CMatrix::CopyOnlyMatrix(matrix);
}

// like previous + attach
CMatrix::CMatrix(RwMatrix* matrix, bool temporary) {
    CMatrix::Attach(matrix, temporary);
}

// destructor detaches matrix if attached
CMatrix::~CMatrix()
{
    CMatrix::Detach();
}

void CMatrix::Attach(RwMatrix* matrix, bool bOwnsMatrix)
{
    CMatrix::Detach();

    m_pAttachMatrix = matrix;
    m_bOwnsAttachedMatrix = bOwnsMatrix;
    CMatrix::Update();
}

void CMatrix::Detach()
{
    if (m_bOwnsAttachedMatrix && m_pAttachMatrix)
        RwMatrixDestroy(m_pAttachMatrix);

    m_pAttachMatrix = nullptr;
}

// copy base RwMatrix to another matrix
void CMatrix::CopyOnlyMatrix(const CMatrix& matrix)
{
    memcpy(this, &matrix, sizeof(RwMatrix));
}

// update RwMatrix with attaching matrix. This doesn't check if attaching matrix is present, so use it only if you know it is present.
// Using UpdateRW() is more safe since it perform this check.
void CMatrix::Update()
{
    CMatrix::UpdateMatrix(m_pAttachMatrix);
}

// update RwMatrix with attaching matrix.
void CMatrix::UpdateRW()
{
    if (!m_pAttachMatrix)
        return;

    CMatrix::UpdateRwMatrix(m_pAttachMatrix);
}

// update RwMatrix with this matrix
void CMatrix::UpdateRwMatrix(RwMatrix* matrix) const
{
    *RwMatrixGetRight(matrix) = right;
    *RwMatrixGetUp(matrix) = up;
    *RwMatrixGetAt(matrix) = at;
    *RwMatrixGetPos(matrix) = pos;

    RwMatrixUpdate(matrix);
}

void CMatrix::UpdateMatrix(RwMatrix* rwMatrix)
{
    right = *RwMatrixGetRight(rwMatrix);
    up = *RwMatrixGetUp(rwMatrix);
    at = *RwMatrixGetAt(rwMatrix);
    pos = *RwMatrixGetPos(rwMatrix);
}

void CMatrix::SetUnity()
{
    CMatrix::ResetOrientation();
    pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::ResetOrientation()
{
    right.Set  (1.0F, 0.0F, 0.0F);
    up.Set(0.0F, 1.0F, 0.0F);
    at.Set     (0.0F, 0.0F, 1.0F);
}

void CMatrix::SetScale(float scale)
{
    right.Set  (scale, 0.0F, 0.0F);
    up.Set(0.0F, scale, 0.0F);
    at.Set     (0.0F, 0.0F, scale);
    pos.Set    (0.0F, 0.0F, 0.0F);
}

// scale on three axes
void CMatrix::SetScale(float x, float y, float z)
{
    right.Set  (x, 0.0F, 0.0F);
    up.Set(0.0F, y, 0.0F);
    at.Set     (0.0F, 0.0F, z   );
    pos.Set    (0.0F, 0.0F, 0.0F);
}

void CMatrix::SetTranslateOnly(CVector translation)
{
    pos = translation;
}

// like previous + reset orientation
void CMatrix::SetTranslate(CVector translation)
{
    CMatrix::ResetOrientation();
    CMatrix::SetTranslateOnly(translation);
}

void CMatrix::SetRotateXOnly(float angle)
{
    auto fSin = sin(angle);
    auto fCos = cos(angle);

    right.Set  (1.0F, 0.0F, 0.0F);
    up.Set(0.0F, fCos, fSin);
    at.Set     (0.0F, -fSin, fCos);
}

void CMatrix::SetRotateYOnly(float angle)
{
    auto fSin = sin(angle);
    auto fCos = cos(angle);

    right.Set  (fCos, 0.0F, -fSin);
    up.Set(0.0F, 1.0F, 0.0F);
    at.Set     (fSin, 0.0F, fCos);
}

void CMatrix::SetRotateZOnly(float angle)
{
    auto fSin = sin(angle);
    auto fCos = cos(angle);

    right.Set  (fCos, fSin, 0.0F);
    up.Set(-fSin, fCos, 0.0F);
    at.Set     (0.0F, 0.0F, 1.0F);
}

void CMatrix::SetRotateX(float angle)
{
    CMatrix::SetRotateXOnly(angle);
    pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::SetRotateY(float angle)
{
    CMatrix::SetRotateYOnly(angle);
    pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::SetRotateZ(float angle)
{
    CMatrix::SetRotateZOnly(angle);
    pos.Set(0.0F, 0.0F, 0.0F);
}

// set rotate on 3 axes
void CMatrix::SetRotate(float x, float y, float z)
{
    auto fSinX = sin(x);
    auto fCosX = cos(x);
    auto fSinY = sin(y);
    auto fCosY = cos(y);
    auto fSinZ = sin(z);
    auto fCosZ = cos(z);

    right.Set  (fCosZ * fCosY - (fSinZ * fSinX) * fSinY, fCosZ * fSinX * fSinY + fSinZ * fCosY, -(fSinY * fCosX));
    up.Set(-(fSinZ * fCosX), fCosZ * fCosX, fSinX);
    at.Set     (fCosZ * fSinY + fSinZ * fSinX * fCosY, fSinZ * fSinY - fCosZ * fSinX * fCosY, fCosY * fCosX);
    pos.Set(0.0F, 0.0F, 0.0F);
}

void CMatrix::RotateX(float angle)
{
    auto rotMat = CMatrix();
    rotMat.SetRotateX(angle);
    right = rotMat * right;
    up = rotMat * up;
    at = rotMat * at;
    pos = rotMat * pos;
}

void CMatrix::RotateY(float angle)
{
    auto rotMat = CMatrix();
    rotMat.SetRotateY(angle);
    right = rotMat * right;
    up = rotMat * up;
    at = rotMat * at;
    pos = rotMat * pos;
}

void CMatrix::RotateZ(float angle)
{
    auto rotMat = CMatrix();
    rotMat.SetRotateZ(angle);
    right = rotMat * right;
    up = rotMat * up;
    at = rotMat * at;
    pos = rotMat * pos;
}

// rotate on 3 axes
void CMatrix::Rotate(CVector rotation)
{
    auto rotMat = CMatrix();
    rotMat.SetRotate(rotation.x, rotation.y, rotation.z);
    right = rotMat * right;
    up = rotMat * up;
    at = rotMat * at;
    pos = rotMat * pos;
}

void CMatrix::Reorthogonalise()
{
    auto vecCross = CrossProduct(right, up);
    vecCross.Normalise();

    auto vecCross2 = CrossProduct(up, vecCross);
    vecCross2.Normalise();

    auto vecCross3 = CrossProduct(vecCross, vecCross2);

    right = vecCross2;
    up = vecCross3;
    at = vecCross;
}

// similar to UpdateRW(RwMatrixTag *)
void CMatrix::CopyToRwMatrix(RwMatrix* matrix) const
{
    UpdateRwMatrix(matrix);
    RwMatrixUpdate(matrix);
}

void CMatrix::SetRotate(const CQuaternion& quat)
{
    auto vecImag2 = quat.imag + quat.imag;
    auto x2x = vecImag2.x * quat.imag.x;
    auto y2x = vecImag2.y * quat.imag.x;
    auto z2x = vecImag2.z * quat.imag.x;

    auto y2y = vecImag2.y * quat.imag.y;
    auto z2y = vecImag2.z * quat.imag.y;
    auto z2z = vecImag2.z * quat.imag.z;

    auto x2r = vecImag2.x * quat.real;
    auto y2r = vecImag2.y * quat.real;
    auto z2r = vecImag2.z * quat.real;

    right.Set  (1.0F - (z2z + y2y), z2r + y2x, z2x - y2r);
    up.Set(y2x - z2r, 1.0F - (z2z + x2x), x2r + z2y);
    at.Set     (y2r + z2x, z2y - x2r, 1.0F - (y2y + x2x));
}

void CMatrix::Scale(float scale) {
    ScaleXYZ(scale, scale, scale);
}

void CMatrix::ScaleXYZ(float x, float y, float z) {
    right   *= x;
    up *= y;
    at      *= z;
}

void CMatrix::ForceUpVector(CVector vecUp) {
    right   = CrossProduct(up, vecUp);
    up = CrossProduct(vecUp, right);
    at      = vecUp;
}

void CMatrix::ConvertToEulerAngles(float* pX, float* pY, float* pZ, uint32_t uiFlags)
{
    float fArr[3][3];

    fArr[0][0] = right.x;
    fArr[0][1] = right.y;
    fArr[0][2] = right.z;

    fArr[1][0] = up.x;
    fArr[1][1] = up.y;
    fArr[1][2] = up.z;

    fArr[2][0] = at.x;
    fArr[2][1] = at.y;
    fArr[2][2] = at.z;

    /* Original indices deciding logic, i replaced it with clearer one
    auto iInd1 = CMatrix::EulerIndices1[(uiFlags >> 3) & 0x3];
    auto iInd2 = CMatrix::EulerIndices2[iInd1 + ((uiFlags & 0x4) != 0)];
    auto iInd3 = CMatrix::EulerIndices2[iInd1 - ((uiFlags & 0x4) != 0) + 1]; */
    int8_t iInd1 = 0, iInd2 = 1, iInd3 = 2;
    switch (uiFlags & eMatrixEulerFlags::_ORDER_MASK) {
        case ORDER_XYZ:
            iInd1 = 0, iInd2 = 1, iInd3 = 2;
            break;
        case ORDER_XZY:
            iInd1 = 0, iInd2 = 2, iInd3 = 1;
            break;
        case ORDER_YZX:
            iInd1 = 1, iInd2 = 2, iInd3 = 0;
            break;
        case ORDER_YXZ:
            iInd1 = 1, iInd2 = 0, iInd3 = 2;
            break;
        case ORDER_ZXY:
            iInd1 = 2, iInd2 = 0, iInd3 = 1;
            break;
        case ORDER_ZYX:
            iInd1 = 2, iInd2 = 1, iInd3 = 0;
            break;
    }

    if (uiFlags & eMatrixEulerFlags::EULER_ANGLES) {
        auto r13 = fArr[iInd1][iInd3];
        auto r12 = fArr[iInd1][iInd2];
        auto cy = sqrt(r12 * r12 + r13 * r13);
        if (cy > 0.0000019073486) { // Some epsilon?
            *pX = atan2(r12, r13);
            *pY = atan2(cy, fArr[iInd1][iInd1]);
            *pZ = atan2(fArr[iInd2][iInd3], -fArr[iInd3][iInd1]);
        }
        else {
            *pX = atan2(-fArr[iInd2][iInd3], fArr[iInd2][iInd2]);
            *pY = atan2(cy, fArr[iInd1][iInd1]);
            *pZ = 0.0F;
        }
    }
    else {
        auto r21 = fArr[iInd2][iInd1];
        auto r11 = fArr[iInd1][iInd1];
        auto cy = sqrt(r11 * r11 + r21 * r21);
        if (cy > 0.0000019073486) { // Some epsilon?
            *pX = atan2(fArr[iInd3][iInd2], fArr[iInd3][iInd3]);
            *pY = atan2(-fArr[iInd3][iInd1], cy);
            *pZ = atan2(r21, r11);
        }
        else {
            *pX = atan2(-fArr[iInd2][iInd3], fArr[iInd2][iInd2]);
            *pY = atan2(-fArr[iInd3][iInd1], cy);
            *pZ = 0.0F;
        }
    }

    if (uiFlags & eMatrixEulerFlags::SWAP_XZ)
        std::swap(*pX, *pZ);

    if (uiFlags & eMatrixEulerFlags::_ORDER_NEEDS_SWAP) {
        *pX = -*pX;
        *pY = -*pY;
        *pZ = -*pZ;
    }
}

void CMatrix::ConvertFromEulerAngles(float x, float y, float z, uint32_t uiFlags)
{
    /* Original indices deciding logic, i replaced it with clearer one
    auto iInd1 = CMatrix::EulerIndices1[(uiFlags >> 3) & 0x3];
    auto iInd2 = CMatrix::EulerIndices2[iInd1 + ((uiFlags & 0x4) != 0)];
    auto iInd3 = CMatrix::EulerIndices2[iInd1 - ((uiFlags & 0x4) != 0) + 1]; */
    int8_t iInd1 = 0, iInd2 = 1, iInd3 = 2;
    switch (uiFlags & eMatrixEulerFlags::_ORDER_MASK) {
        case ORDER_XYZ:
            iInd1 = 0, iInd2 = 1, iInd3 = 2;
            break;
        case ORDER_XZY:
            iInd1 = 0, iInd2 = 2, iInd3 = 1;
            break;
        case ORDER_YZX:
            iInd1 = 1, iInd2 = 2, iInd3 = 0;
            break;
        case ORDER_YXZ:
            iInd1 = 1, iInd2 = 0, iInd3 = 2;
            break;
        case ORDER_ZXY:
            iInd1 = 2, iInd2 = 0, iInd3 = 1;
            break;
        case ORDER_ZYX:
            iInd1 = 2, iInd2 = 1, iInd3 = 0;
            break;
    }

    float fArr[3][3];

    if (uiFlags & eMatrixEulerFlags::SWAP_XZ)
        std::swap(x, z);

    if (uiFlags & eMatrixEulerFlags::_ORDER_NEEDS_SWAP) {
        x = -x;
        y = -y;
        z = -z;
    }

    auto fSinX = sin(x);
    auto fCosX = cos(x);
    auto fSinY = sin(y);
    auto fCosY = cos(y);
    auto fSinZ = sin(z);
    auto fCosZ = cos(z);

    if (uiFlags & eMatrixEulerFlags::EULER_ANGLES) {
        fArr[iInd1][iInd1] = fCosY;
        fArr[iInd1][iInd2] = fSinX*fSinY;
        fArr[iInd1][iInd3] = fCosX*fSinY;

        fArr[iInd2][iInd1] =   fSinY*fSinZ;
        fArr[iInd2][iInd2] =   fCosX*fCosY  - fCosY*fSinX*fSinZ;
        fArr[iInd2][iInd3] = -(fSinX*fCosZ) -(fCosX*fCosY*fSinZ);

        fArr[iInd3][iInd1] = -(fCosZ*fSinY);
        fArr[iInd3][iInd2] =   fCosX*fSinZ  + fCosY*fCosZ*fSinX;
        fArr[iInd3][iInd3] = -(fSinX*fSinZ) + fCosX*fCosY*fCosZ;
    }
    else { // Use Tait-Bryan angles
        fArr[iInd1][iInd1] =   fCosY*fCosZ;
        fArr[iInd1][iInd2] = -(fCosX*fSinZ) + fCosZ*fSinX*fSinY;
        fArr[iInd1][iInd3] =   fSinX*fSinZ  + fCosX*fCosZ*fSinY;

        fArr[iInd2][iInd1] =   fCosY*fSinZ;
        fArr[iInd2][iInd2] =   fCosX*fCosZ  + fSinX*fSinY*fSinZ;
        fArr[iInd2][iInd3] = -(fCosZ*fSinX) + fCosX*fSinY*fSinZ;

        fArr[iInd3][iInd1] = -fSinY;
        fArr[iInd3][iInd2] =  fCosY*fSinX;
        fArr[iInd3][iInd3] =  fCosX*fCosY;
    }

    right.Set  (fArr[0][0], fArr[0][1], fArr[0][2]);
    up.Set(fArr[1][0], fArr[1][1], fArr[1][2]);
    at.Set     (fArr[2][0], fArr[2][1], fArr[2][2]);
}

void CMatrix::operator=(const CMatrix& rvalue)
{
    CMatrix::CopyOnlyMatrix(rvalue);
    CMatrix::UpdateRW();
}

void CMatrix::operator+=(const CMatrix& rvalue)
{
    right += rvalue.right;
    up += rvalue.up;
    at += rvalue.at;
    pos += rvalue.pos;
}

void CMatrix::operator*=(const CMatrix& rvalue)
{
    *this = (*this * rvalue);
}

CMatrix operator*(const CMatrix& a, const CMatrix& b)
{
    auto result = CMatrix();
    result.right = a.right * b.right.x + a.up * b.right.y + a.at * b.right.z;
    result.up = a.right * b.up.x + a.up * b.up.y + a.at * b.up.z;
    result.at = a.right * b.at.x + a.up * b.at.y + a.at * b.at.z;
    result.pos = a.right * b.pos.x + a.up * b.pos.y + a.at * b.pos.z + a.pos;
    return result;
}

CVector operator*(const CMatrix& a, const CVector& b)
{
    CVector result;
    result.x = a.pos.x + a.right.x * b.x + a.up.x * b.y + a.at.x * b.z;
    result.y = a.pos.y + a.right.y * b.x + a.up.y * b.y + a.at.y * b.z;
    result.z = a.pos.z + a.right.z * b.x + a.up.z * b.y + a.at.z * b.z;
    return result;
}
CMatrix operator+(const CMatrix& a, const CMatrix& b)
{
    CMatrix result;
    result.right = a.right + b.right;
    result.up = a.up + b.up;
    result.at = a.at + b.at;
    result.pos = a.pos + b.pos;
    return result;
}

CMatrix& Invert(CMatrix& in, CMatrix& out)
{
    out.GetPosition().Set(0.0F, 0.0F, 0.0F);

    out.GetRight().Set(in.GetRight().x, in.GetForward().x, in.GetUp().x);
    out.GetForward().Set(in.GetRight().y, in.GetForward().y, in.GetUp().y);
    out.GetUp().Set(in.GetRight().z, in.GetForward().z, in.GetUp().z);

    out.GetPosition() += in.GetPosition().x * out.GetRight();
    out.GetPosition() += in.GetPosition().y * out.GetForward();
    out.GetPosition() += in.GetPosition().z * out.GetUp();
    out.GetPosition() *= -1.0F;

    return out;
}

CMatrix Invert(const CMatrix& in)
{
    CMatrix result;
    Invert(const_cast<CMatrix&>(in), result); // const cast necessary because it's fucked - but it wont be modified.
    return result;
}

CMatrix Lerp(CMatrix from, CMatrix to, float t) {
    from.ScaleAll(1.0f - t);
    to.ScaleAll(t);
    return from + to;
}

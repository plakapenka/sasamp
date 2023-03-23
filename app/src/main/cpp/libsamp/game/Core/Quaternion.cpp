#include "main.h"
#include "game/game.h"
#include <cmath>

// Quat to matrix
void CQuaternion::Get(RwMatrix* out) const {
	float sqw = w * w; // v13 = a1 * a1;
	float sqx = x * x; // v14 = a2 * a2;
	float sqy = y * y; // v15 = a3 * a3;
	float sqz = z * z; // v16 = a4 * a4;

	out->right.x = ( sqx - sqy - sqz + sqw); 	// a5 = v14 - v15 - v16 + v13;
	out->up.y = (-sqx + sqy - sqz + sqw);		// a9 = v15 - v14 - v16 + v13;
	out->at.z = (-sqx - sqy + sqz + sqw);		// a13 = v16 - (v15 + v14) + v13;

	float tmp1 = x * y;				// v17 = a2 * a3;
	float tmp2 = z * w;				// v18 = a1 * a4;
	out->up.x = 2.0 * (tmp1 + tmp2);		// a8 = v18 + v17 + v18 + v17;
	out->right.y = 2.0 * (tmp1 - tmp2);		// a6 = v17 - v18 + v17 - v18;

	tmp1 = x*z;						// v20 = a2 * a4;
	tmp2 = y*w;						// v21 = a1 * a3;
	out->at.x = 2.0 * (tmp1 - tmp2);		// a11 = v20 - v21 + v20 - v21;
	out->right.z = 2.0 * (tmp1 + tmp2);		// a7 = v21 + v20 + v21 + v20;
	tmp1 = y*z;						// v22 = a3 * a4;
	tmp2 = x*w;						// v23 = a1 * a2;
	out->at.y = 2.0 * (tmp1 + tmp2);		// a12 = v23 + v22 + v23 + v22;
	out->up.z = 2.0 * (tmp1 - tmp2);		// a10 = v22 - v23 + v22 - v23;
}

// Quat to euler angles
void CQuaternion::Get(float* pax, float* pay, float* paz) {
	float x; // s18
	float z; // s0
	float y; // s20
	float v8; // s24
	float w; // s22
	float v12; // s30
	float v13; // s17
	float v14; // s19
	float v15; // r0
	float v16; // s27
	float v17; // r5
	float v18; // s29
	float v19; // s26
	float v20; // s0
	float v21; // r0
	float v22; // s0

	x = this->x;
	z = this->z;
	y = this->y;
	v8 = z + z;
	w = this->w;
	v12 = z * (float)(z + z);
	v13 = w * (float)(z + z);
	v14 = this->x * (float)(y + y);
	v15 = atan2f(v14 + v13, 1.0 - (float)((float)(x * (float)(x + x)) + v12));
	v16 = v15;
	if ( v15 < 0.0 )
		v16 = v15 + 6.2832;
	v17 = sinf(v16);
	v18 = cosf(v16);
	*paz = v16;
	v19 = (float)(x + x) * w;
	v20 = atan2f(
			-(float)((float)(y * v8) - v19),
			(float)((float)(v14 + (float)(w * (float)(z + z))) * v17)
			+ (float)((float)(1.0 - (float)((float)(x * (float)(x + x)) + v12)) * v18));
	if ( v20 < 0.0 )
		v20 = v20 + 6.2832;
	*pax = v20;
	v21 = atan2f(
			-(float)((float)((float)((float)(x * v8) - (float)((float)(y + y) * w)) * v18)
					 - (float)((float)((float)(y * v8) + v19) * v17)),
			(float)((float)(1.0 - (float)((float)(y * (float)(y + y)) + v12)) * v18) - (float)((float)(v14 - v13) * v17));
	v22 = v21;
	if ( v21 < 0.0 )
		v22 = v21 + 6.2832;
	*pay = v22;
}

// Quat to axis & angle
void CQuaternion::Get(RwV3d* pVec, float* fRotation) {
	float v6; // r4
	float v7; // r0

	v6 = acosf(this->w + this->w);
	v7 = sinf(v6);
	*fRotation = v6;
	pVec->x = this->x * (float)(1.0 / v7);
	pVec->y = (float)(1.0 / v7) * this->y;
	pVec->z = (float)(1.0 / v7) * this->z;
}

// Stores result of quat multiplication
void CQuaternion::Multiply(const CQuaternion* quat1, const CQuaternion* quat2) {
	float v3; // s0
	float v4; // s2
	float v5; // s4

	v3 = (float)(quat1->y * quat2->z) - (float)(quat2->y * quat1->z);
	this->x = v3;
	v4 = (float)(quat1->z * quat2->x) - (float)(quat2->z * quat1->x);
	this->y = v4;
	v5 = (float)(quat1->x * quat2->y) - (float)(quat2->x * quat1->y);
	this->z = v5;
	this->x = v3 + (float)((float)(quat1->x * quat2->w) + (float)(quat2->x * quat1->w));
	this->y = v4 + (float)((float)(quat1->y * quat2->w) + (float)(quat2->y * quat1->w));
	this->z = v5 + (float)((float)(quat1->z * quat2->w) + (float)(quat2->z * quat1->w));
	this->w = (float)(quat1->w * quat2->w)
			  - (float)((float)((float)(quat1->x * quat2->x) + (float)(quat1->y * quat2->y)) + (float)(quat1->z * quat2->z));
}

// Quat from matrix
void CQuaternion::Set(const RwMatrix *mat) {
	w = sqrt( std::max( (float)0, 1.0f + mat->right.x + mat->up.y + mat->at.z ) ) * 0.5f;
	x = sqrt( std::max( (float)0, 1.0f + mat->right.x - mat->up.y - mat->at.z ) ) * 0.5f;
	y = sqrt( std::max( (float)0, 1.0f - mat->right.x + mat->up.y - mat->at.z ) ) * 0.5f;
	z = sqrt( std::max( (float)0, 1.0f - mat->right.x - mat->up.y + mat->at.z ) ) * 0.5f;

	x = static_cast < float > ( copysign( x, mat->at.y - mat->up.z ) );
	y = static_cast < float > ( copysign( y, mat->right.z - mat->at.x ) );
	z = static_cast < float > ( copysign( z, mat->up.x - mat->right.y ) );
	//((void(__thiscall*)(CQuaternion*, const RwMatrix&))0x59C3E0)(this, m);
}

// Quat from euler angles
void CQuaternion::Set(float ax, float ay, float az) {
	float v7; // r5
	float v8; // r9
	float v9; // r6
	float v10; // r10
	float v11; // r5
	float v12; // s16
	float v13; // s24
	float v14; // s28
	float v15; // s16
	float v16; // r0

	v7 = ax * 0.5;
	v8 = cosf(ax * 0.5);
	v9 = ay * 0.5;
	v10 = cosf(v9);
	v11 = sinf(v7);
	v12 = sinf(v9);
	v13 = v11 * v12;
	v14 = cosf(az * 0.5);
	v15 = v8 * v12;
	v16 = sinf(az * 0.5);
	this->x = (float)((float)(v8 * v10) * v16) - (float)(v14 * v13);
	this->y = (float)(v14 * (float)(v10 * v11)) + (float)(v16 * v15);
	this->z = (float)(v14 * v15) - (float)((float)(v10 * v11) * v16);
	this->w = (float)(v14 * (float)(v8 * v10)) + (float)(v16 * v13);
}

// Quat from axis & angle
void CQuaternion::Set(RwV3d* pVec, float fRotation) {
	float v5; // r6
	float v6; // s16
	float v7; // r0
	float z; // s0

	v5 = fRotation * 0.5;
	v6 = sinf(fRotation * 0.5);
	this->x = v6 * pVec->x;
	this->y = v6 * pVec->y;
	v7 = cosf(v5);
	z = pVec->z;
	this->w = v7;
	this->z = v6 * z;
}

// Conjugate of a quat
void CQuaternion::Conjugate() {
	((void(__thiscall*)(CQuaternion*))0x4D37D0)(this);
}

// Squared length of a quat
float CQuaternion::GetLengthSquared() const {
	// Originally NOP.
	return sq(x) + sq(y) + sq(z) + sq(w);
}

// Multiplies quat by a floating point value
void CQuaternion::Scale(float multiplier) {
	((void(__thiscall*)(CQuaternion*, float))0x4CF9B0)(this, multiplier);
}

// Copies value from other quat
void CQuaternion::Copy(const CQuaternion& from) {
	((void(__thiscall*)(CQuaternion*, const CQuaternion&))0x4CF9E0)(this, from);
}

// Gets a dot product for quats
void CQuaternion::Dot(const CQuaternion& a) {
	((void(__thiscall*)(CQuaternion*, const CQuaternion&))0x4CFA00)(this, a);
}


void CQuaternion::Normalise()
{
	double n = sqrt(x*x + y*y + z*z + w*w);

	w /= n;
	x /= n;
	y /= n;
	z /= n;
}

// Spherical linear interpolation
void CQuaternion::Slerp(const CQuaternion& quatStart, const CQuaternion& quatEnd, float fTheta, float fOOSinTheta, float fInterpValue)
{
//	float v10; // s20
//	float v11; // r8
//	float v12; // r0
//
//	if ( fTheta == 0.0 )
//	{
//		*this = *quatEnd;
//	}
//	else
//	{
//		if ( fTheta <= 1.5708 )
//		{
//			v11 = sinf((float)(1.0 - fInterpValue) * fTheta);
//			v12 = sinf(fTheta * fInterpValue);
//		}
//		else
//		{
//			v10 = 3.1416 - fTheta;
//			v11 = sinf((float)(3.1416 - fTheta) * (float)(1.0 - fInterpValue));
//			LODWORD(v12) = COERCE_UNSIGNED_INT(sinf(v10 * fInterpValue)) ^ 0x80000000;
//		}
//		this->x = (float)((float)(v11 * fOOSinTheta) * quatStart->x) + (float)((float)(v12 * fOOSinTheta) * quatEnd->x);
//		this->y = (float)((float)(v11 * fOOSinTheta) * quatStart->y) + (float)((float)(v12 * fOOSinTheta) * quatEnd->y);
//		this->z = (float)((float)(v11 * fOOSinTheta) * quatStart->z) + (float)((float)(v12 * fOOSinTheta) * quatEnd->z);
//		this->w = (float)((float)(v11 * fOOSinTheta) * quatStart->w) + (float)((float)(v12 * fOOSinTheta) * quatEnd->w);
//	}
	((void(__thiscall*)(CQuaternion*, const CQuaternion&, const CQuaternion&, float, float, float))(g_libGTASA + 0x00450634 + 1))(this, quatStart, quatEnd, fTheta, fOOSinTheta, fInterpValue);
}

#define SLERP_DELTA 0.1
// Spherical linear interpolation
void CQuaternion::Slerp(const CQuaternion* pQ1, const CQuaternion* pQ2, float t) {
	float p1[4];
	double omega, cosom, sinom, scale0, scale1;
	cosom = pQ1->x*pQ2->x + pQ1->y*pQ2->y + pQ1->z*pQ2->z + pQ1->w*pQ2->w;

	if(cosom < 0.0)
	{
		cosom = -cosom;
		p1[0] = - pQ2->x;  p1[1] = - pQ2->y;
		p1[2] = - pQ2->z;  p1[3] = - pQ2->w;
	}
	else
	{
		p1[0] = pQ2->x;    p1[1] = pQ2->y;
		p1[2] = pQ2->z;    p1[3] = pQ2->w;
	}

	if((1.0 - cosom) > SLERP_DELTA)
	{
		// стандартный случай (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
	}
	else
	{
		// если маленький угол - линейная интерполяция
		scale0 = 1.0 - t;
		scale1 = t;
	}
	Set(scale0 * pQ1->x + scale1 * p1[0],
		scale0 * pQ1->y + scale1 * p1[1],
		scale0 * pQ1->z + scale1 * p1[2],
		scale0 * pQ1->w + scale1 * p1[3]);
	//((void(__thiscall*)(CQuaternion*, const CQuaternion*, const CQuaternion*, float))(g_libGTASA + 0x0045074C + 1))(this, quatStart, quatEnd, fInterpValue);
}
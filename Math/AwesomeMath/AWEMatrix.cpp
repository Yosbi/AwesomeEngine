//-------------------------------------------------------------------------------
// AWEMatrix.cpp
// Yosbi Alves
// yosbi@outlook.com
// Copyright (c) 2023
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		AWEMatrix class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "AwesomeMath.h"     
//--------------------------------------------------------------------
// Name: AWEMatrix()
// Desc: Constructor
//--------------------------------------------------------------------
AWEMatrix::AWEMatrix()
{
	// Set a default identity matrix
	this->Identity();
}
//--------------------------------------------------------------------
// Name: AWEMatrix()
// Desc: Transform a matrix in a identity matrix
//--------------------------------------------------------------------
inline void AWEMatrix::Identity(void)
{
	float* f = (float*)&this->_11;
	memset(f, 0, sizeof(AWEMatrix));
	_11 = _22 = _33 = _44 = 1.0f;
}

//--------------------------------------------------------------------
// Name: RotaX()
// Desc: Build rotation matrix around X axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaX(float a) {
	float fCos = cosf(a);
	float fSin = sinf(a);

	_22 = fCos;
	_23 = fSin;
	_32 = -fSin;
	_33 = fCos;

	_11 = _44 = 1.0f;
	_12 = _13 = _14 = _21 = _24 = _31 = _34 = _41 = _42 = _43 = 0.0f;
}

//--------------------------------------------------------------------
// Name: RotaY()
// Desc: Build rotation matrix around Y axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaY(float a) {
	float fCos = cosf(a);
	float fSin = sinf(a);

	_11 = fCos;
	_13 = -fSin;
	_31 = fSin;
	_33 = fCos;

	_22 = _44 = 1.0f;
	_12 = _23 = _14 = _21 = _24 = _32 = _34 = _41 = _42 = _43 = 0.0f;
}

//--------------------------------------------------------------------
// Name: RotaZ()
// Desc: Build rotation matrix around Z axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaZ(float a) {
	float fCos = cosf(a);
	float fSin = sinf(a);

	_11 = fCos;
	_12 = fSin;
	_21 = -fSin;
	_22 = fCos;

	_33 = _44 = 1.0f;
	_13 = _14 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
}

//--------------------------------------------------------------------
// Name: YApplyInverseRota()
// Desc: Apply an inverse rotation arround a vector
//--------------------------------------------------------------------
inline void AWEMatrix::ApplyInverseRota(AWEVector* pvc) {
	pvc->x = pvc->x * _11 + pvc->y * _12 + pvc->z * _13;
	pvc->y = pvc->x * _21 + pvc->y * _22 + pvc->z * _23;
	pvc->z = pvc->x * _31 + pvc->y * _32 + pvc->z * _33;
	pvc->w = 1.0f;
}

//--------------------------------------------------------------------
// Name: YRota()
// Desc: Apply a rotation arround x,y,z angles
//--------------------------------------------------------------------
inline void AWEMatrix::Rota(float x, float y, float z)
{
	Rota(AWEVector(x, y, z));
}

//--------------------------------------------------------------------
// Name: YRota()
// Desc: Apply a rotation arround the vector
//--------------------------------------------------------------------
inline void AWEMatrix::Rota(const AWEVector& vc) {
	float sr, sp, sy, cr, cp, cy;

	Identity();

	sy = sinf(vc.z);
	cy = cosf(vc.z);
	sp = sinf(vc.y);
	cp = cosf(vc.y);
	sr = sinf(vc.x);
	cr = cosf(vc.x);

	_11 = cp * cy;
	_12 = cp * sy;
	_13 = -sp;
	_21 = sr * sp * cy + cr * -sy;
	_22 = sr * sp * sy + cr * cy;
	_23 = sr * cp;
	_31 = (cr * sp * cy + -sr * -sy);
	_32 = (cr * sp * sy + -sr * cy);
	_33 = cr * cp;
}

//--------------------------------------------------------------------
// Name: YSetTranslation()
// Desc: Set translation values,  if the boolean b is true, then
//		the matrix is set first to an indentity matrix
//--------------------------------------------------------------------
inline void AWEMatrix::SetTranslation(AWEVector vc, bool b) {
	if (b) Identity();
	_41 = vc.x;
	_42 = vc.y;
	_43 = vc.z;
} // SetTranslation

//--------------------------------------------------------------------
// Name: GetTranslation()
// Desc: Get translation values
//--------------------------------------------------------------------
inline AWEVector AWEMatrix::GetTranslation(void)
{
	return AWEVector(_41, _42, _43);
}

//--------------------------------------------------------------------
// Name: RotaArbi()
// Desc: Build rotation matrix around arbitrary axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaArbi(const AWEVector& _vcAxis, float a) {
	AWEVector vcAxis = _vcAxis;
	float fCos = cosf(a);
	float fSin = sinf(a);
	float fSum = 1.0f - fCos;

	if (vcAxis.GetSqrLength() != 1.0)
		vcAxis.Normalize();

	_11 = (vcAxis.x * vcAxis.x) * fSum + fCos;
	_12 = (vcAxis.x * vcAxis.y) * fSum - (vcAxis.z * fSin);
	_13 = (vcAxis.x * vcAxis.z) * fSum + (vcAxis.y * fSin);

	_21 = (vcAxis.y * vcAxis.x) * fSum + (vcAxis.z * fSin);
	_22 = (vcAxis.y * vcAxis.y) * fSum + fCos;
	_23 = (vcAxis.y * vcAxis.z) * fSum - (vcAxis.x * fSin);

	_31 = (vcAxis.z * vcAxis.x) * fSum - (vcAxis.y * fSin);
	_32 = (vcAxis.z * vcAxis.y) * fSum + (vcAxis.x * fSin);
	_33 = (vcAxis.z * vcAxis.z) * fSum + fCos;

	_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
	_44 = 1.0f;
}

//--------------------------------------------------------------------
// Name: Translate()
// Desc: Add translation to matrix
//--------------------------------------------------------------------
inline void AWEMatrix::Translate(float dx, float dy, float dz) {
	_41 = dx;
	_42 = dy;
	_43 = dz;
}

//--------------------------------------------------------------------
// Name: LookAt()
// Desc: look from given position at given point
//--------------------------------------------------------------------

inline void AWEMatrix::LookAt(AWEVector vcPos, AWEVector vcLookAt, AWEVector vcWorldUp)
{
	AWEVector vcDir = vcLookAt - vcPos;
	AWEVector vcUp, vcRight;
	float   fAngle = 0.0f;

	vcDir.Normalize();

	fAngle = vcWorldUp * vcDir;

	vcUp = vcWorldUp - (vcDir * fAngle);
	vcUp.Normalize();

	vcRight = vcUp.Cross(vcDir);

	_11 = vcRight.x; _21 = vcUp.x; _31 = vcDir.x;
	_12 = vcRight.y; _22 = vcUp.y; _32 = vcDir.y;
	_13 = vcRight.z; _23 = vcUp.z; _33 = vcDir.z;

	_41 = vcPos.x;
	_42 = vcPos.y;
	_43 = vcPos.z;

	_14 = 0.0f; _24 = 0.0f; _34 = 0.0f; _44 = 1.0f;
}

//--------------------------------------------------------------------
// Name: Operator * ()
// Desc: Multiply two matrices
//-------------------------------------------------------------------- 
AWEMatrix AWEMatrix::operator * (const AWEMatrix& m) const {
	AWEMatrix result;

	result._11 = _11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41;
	result._12 = _11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42;
	result._13 = _11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43;
	result._14 = _11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44;

	result._21 = _21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41;
	result._22 = _21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42;
	result._23 = _21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43;
	result._24 = _21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44;

	result._31 = _31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41;
	result._32 = _31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42;
	result._33 = _31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43;
	result._34 = _31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44;

	result._41 = _41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41;
	result._42 = _41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42;
	result._43 = _41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43;
	result._44 = _41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44;

	return result;
}

//--------------------------------------------------------------------
// Name: Operator * ()
// Desc: Multiply matrix with a vector
//-------------------------------------------------------------------- 
AWEVector AWEMatrix::operator * (const AWEVector& vc) const {
	AWEVector vcResult;


	vcResult.x = vc.x * _11 + vc.y * _21 + vc.z * _31 + _41;
	vcResult.y = vc.x * _12 + vc.y * _22 + vc.z * _32 + _42;
	vcResult.z = vc.x * _13 + vc.y * _23 + vc.z * _33 + _43;
	vcResult.w = vc.x * _14 + vc.y * _24 + vc.z * _34 + _44;

	vcResult.x = vcResult.x / vcResult.w;
	vcResult.y = vcResult.y / vcResult.w;
	vcResult.z = vcResult.z / vcResult.w;
	vcResult.w = 1.0f;
	
	return vcResult;
}

//--------------------------------------------------------------------
// Name: TransposeOf()
// Desc: Transpose the matrix
//--------------------------------------------------------------------
inline void AWEMatrix::TransposeOf(const AWEMatrix& m) {
	_11 = m._11;
	_21 = m._12;
	_31 = m._13;
	_41 = m._14;

	_12 = m._21;
	_22 = m._22;
	_32 = m._23;
	_42 = m._24;

	_13 = m._31;
	_23 = m._32;
	_33 = m._33;
	_43 = m._34;

	_14 = m._41;
	_24 = m._42;
	_34 = m._43;
	_44 = m._44;
}

//--------------------------------------------------------------------
// Name: Scale()
// Desc: Make a scale matrix to the d factors
//--------------------------------------------------------------------
inline void AWEMatrix::Scale(float dx, float dy, float dz)
{
	_11 = dx; _22 = dy; _33 = dz; _44 = 1.0f;
}

//--------------------------------------------------------------------
// Name: InverseOf()
// Desc: invert the matrix, use of intel's SSE code is incredibly slow here.
//--------------------------------------------------------------------
inline void AWEMatrix::InverseOf(const AWEMatrix& m) {
	//   if (!g_bSSE) {
	AWEMatrix mTrans;
	float     fTemp[12],  // cofactors
		fDet;

	// calculate transposed matrix
	mTrans.TransposeOf(m);

	// Pairs for the first 8 cofactors
	fTemp[0] = mTrans._33 * mTrans._44;
	fTemp[1] = mTrans._34 * mTrans._43;
	fTemp[2] = mTrans._32 * mTrans._44;
	fTemp[3] = mTrans._34 * mTrans._42;
	fTemp[4] = mTrans._32 * mTrans._43;
	fTemp[5] = mTrans._33 * mTrans._42;
	fTemp[6] = mTrans._31 * mTrans._44;
	fTemp[7] = mTrans._34 * mTrans._41;
	fTemp[8] = mTrans._31 * mTrans._43;
	fTemp[9] = mTrans._33 * mTrans._41;
	fTemp[10] = mTrans._31 * mTrans._42;
	fTemp[11] = mTrans._32 * mTrans._41;

	// Calculate the first 8 cofactors
	this->_11 = fTemp[0] * mTrans._22 + fTemp[3] * mTrans._23 + fTemp[4] * mTrans._24;
	this->_11 -= fTemp[1] * mTrans._22 + fTemp[2] * mTrans._23 + fTemp[5] * mTrans._24;
	this->_12 = fTemp[1] * mTrans._21 + fTemp[6] * mTrans._23 + fTemp[9] * mTrans._24;
	this->_12 -= fTemp[0] * mTrans._21 + fTemp[7] * mTrans._23 + fTemp[8] * mTrans._24;
	this->_13 = fTemp[2] * mTrans._21 + fTemp[7] * mTrans._22 + fTemp[10] * mTrans._24;
	this->_13 -= fTemp[3] * mTrans._21 + fTemp[6] * mTrans._22 + fTemp[11] * mTrans._24;
	this->_14 = fTemp[5] * mTrans._21 + fTemp[8] * mTrans._22 + fTemp[11] * mTrans._23;
	this->_14 -= fTemp[4] * mTrans._21 + fTemp[9] * mTrans._22 + fTemp[10] * mTrans._23;
	this->_21 = fTemp[1] * mTrans._12 + fTemp[2] * mTrans._13 + fTemp[5] * mTrans._14;
	this->_21 -= fTemp[0] * mTrans._12 + fTemp[3] * mTrans._13 + fTemp[4] * mTrans._14;
	this->_22 = fTemp[0] * mTrans._11 + fTemp[7] * mTrans._13 + fTemp[8] * mTrans._14;
	this->_22 -= fTemp[1] * mTrans._11 + fTemp[6] * mTrans._13 + fTemp[9] * mTrans._14;
	this->_23 = fTemp[3] * mTrans._11 + fTemp[6] * mTrans._12 + fTemp[11] * mTrans._14;
	this->_23 -= fTemp[2] * mTrans._11 + fTemp[7] * mTrans._12 + fTemp[10] * mTrans._14;
	this->_24 = fTemp[4] * mTrans._11 + fTemp[9] * mTrans._12 + fTemp[10] * mTrans._13;
	this->_24 -= fTemp[5] * mTrans._11 + fTemp[8] * mTrans._12 + fTemp[11] * mTrans._13;

	// Pairs for the second 8 cofactors
	fTemp[0] = mTrans._13 * mTrans._24;
	fTemp[1] = mTrans._14 * mTrans._23;
	fTemp[2] = mTrans._12 * mTrans._24;
	fTemp[3] = mTrans._14 * mTrans._22;
	fTemp[4] = mTrans._12 * mTrans._23;
	fTemp[5] = mTrans._13 * mTrans._22;
	fTemp[6] = mTrans._11 * mTrans._24;
	fTemp[7] = mTrans._14 * mTrans._21;
	fTemp[8] = mTrans._11 * mTrans._23;
	fTemp[9] = mTrans._13 * mTrans._21;
	fTemp[10] = mTrans._11 * mTrans._22;
	fTemp[11] = mTrans._12 * mTrans._21;

	// Calculate the second 8 cofactors
	this->_31 = fTemp[0] * mTrans._42 + fTemp[3] * mTrans._43 + fTemp[4] * mTrans._44;
	this->_31 -= fTemp[1] * mTrans._42 + fTemp[2] * mTrans._43 + fTemp[5] * mTrans._44;
	this->_32 = fTemp[1] * mTrans._41 + fTemp[6] * mTrans._43 + fTemp[9] * mTrans._44;
	this->_32 -= fTemp[0] * mTrans._41 + fTemp[7] * mTrans._43 + fTemp[8] * mTrans._44;
	this->_33 = fTemp[2] * mTrans._41 + fTemp[7] * mTrans._42 + fTemp[10] * mTrans._44;
	this->_33 -= fTemp[3] * mTrans._41 + fTemp[6] * mTrans._42 + fTemp[11] * mTrans._44;
	this->_34 = fTemp[5] * mTrans._41 + fTemp[8] * mTrans._42 + fTemp[11] * mTrans._43;
	this->_34 -= fTemp[4] * mTrans._41 + fTemp[9] * mTrans._42 + fTemp[10] * mTrans._43;
	this->_41 = fTemp[2] * mTrans._33 + fTemp[5] * mTrans._34 + fTemp[1] * mTrans._32;
	this->_41 -= fTemp[4] * mTrans._34 + fTemp[0] * mTrans._32 + fTemp[3] * mTrans._33;
	this->_42 = fTemp[8] * mTrans._34 + fTemp[0] * mTrans._31 + fTemp[7] * mTrans._33;
	this->_42 -= fTemp[6] * mTrans._33 + fTemp[9] * mTrans._34 + fTemp[1] * mTrans._31;
	this->_43 = fTemp[6] * mTrans._32 + fTemp[11] * mTrans._34 + fTemp[3] * mTrans._31;
	this->_43 -= fTemp[10] * mTrans._34 + fTemp[2] * mTrans._31 + fTemp[7] * mTrans._32;
	this->_44 = fTemp[10] * mTrans._33 + fTemp[4] * mTrans._31 + fTemp[9] * mTrans._32;
	this->_44 -= fTemp[8] * mTrans._32 + fTemp[11] * mTrans._33 + fTemp[5] * mTrans._31;

	fDet = mTrans._11 * this->_11 +
		mTrans._12 * this->_12 +
		mTrans._13 * this->_13 +
		mTrans._14 * this->_14;

	fDet = 1 / fDet;

	this->_11 *= fDet;
	this->_12 *= fDet;
	this->_13 *= fDet;
	this->_14 *= fDet;

	this->_21 *= fDet;
	this->_22 *= fDet;
	this->_23 *= fDet;
	this->_24 *= fDet;

	this->_31 *= fDet;
	this->_32 *= fDet;
	this->_33 *= fDet;
	this->_34 *= fDet;

	this->_41 *= fDet;
	this->_42 *= fDet;
	this->_43 *= fDet;
	this->_44 *= fDet;
}

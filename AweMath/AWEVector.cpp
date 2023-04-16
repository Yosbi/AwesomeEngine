//-------------------------------------------------------------------------------
// AWEVector.cpp
// Yosbi Alves
// yosbi@outlook.com
// Copyright (c) 2023
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		YVector class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "AWEMath.h"

//--------------------------------------------------------------------
// Name: _fabs()
// Desc: Returns a float positive (absolut value)
//--------------------------------------------------------------------
float _fabs(float f) { if (f < 0.0f) return -f; return f; }

//--------------------------------------------------------------------
// Name: Set()
// Desc: Sets the vector with the values passed
//--------------------------------------------------------------------
inline void AWEVector::Set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

//--------------------------------------------------------------------
// Name: GetLength()
// Desc: returns the length of the vector(magnitude)
//--------------------------------------------------------------------
inline float AWEVector::GetLength()
{
	return (float)sqrt(x * x + y * y + z * z); // not using w
}

//--------------------------------------------------------------------
// Name: GetSqrLength()
// Desc: returns the length of the vector(magnitude) without sqrt
//--------------------------------------------------------------------
inline float AWEVector::GetSqrLength() const
{
	return (x * x, y * y, z * z);
}

//--------------------------------------------------------------------
// Name: Negate()
// Desc: returns a vector negated
//--------------------------------------------------------------------
inline void AWEVector::Negate()
{
	x = -x;
	y = -y;
	z = -z;

}

//--------------------------------------------------------------------
// Name: AngleWith()
// Desc: returns the angle between two vectors
//--------------------------------------------------------------------
inline float AWEVector::AngleWith(AWEVector& v)
{
	return (float)acos(((*this) * v) / (this->GetLength() * v.GetLength()));
}

//--------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize the vector
//--------------------------------------------------------------------
inline void AWEVector::Normalize()
{
	float length = GetLength();
	if (length > 0)
	{
		float invLength = 1.0f / length;
		x *= invLength;
		y *= invLength;
		z *= invLength;
	}
	w = 1.0f;
}

//--------------------------------------------------------------------
// Name: Difference()
// Desc: Get the vector difference from v1 to v2
//--------------------------------------------------------------------
inline void	AWEVector::Difference(const AWEVector& v1, const AWEVector& v2)
{
	x = v2.x - v1.x;
	y = v2.y - v1.y;
	z = v2.z - v1.z;
	w = 1.0;
}

//--------------------------------------------------------------------
// Name: Cross()
// Desc: Build cross prodict of two vectors.
//--------------------------------------------------------------------
inline AWEVector AWEVector::Cross(const AWEVector& v2)
{
	float xResult = y * v2.z - z * v2.y;
	float yResult = z * v2.x - x * v2.z;
	float zResult = x * v2.y - y * v2.x;

	return AWEVector(xResult, yResult, zResult);
}

//--------------------------------------------------------------------
// Name: RotateWith()
// Desc: Rotate the vector with the given matrix, ignoring translation
//		 information in matrix if they are present
//--------------------------------------------------------------------
inline void AWEVector::RotateWith(const AWEMatrix& m)
{
	// Applying rotational part of matrix only
	float _x = x * m._11 + y * m._21 + z * m._31;
	float _y = x * m._12 + y * m._22 + z * m._32;
	float _z = x * m._13 + y * m._23 + z * m._33;
	x = _x;   y = _y;   z = _z;
}

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Rotate the vector with the inverse rotation part of the given
//		 matrix, ignoring translation of the matrix if present.
//--------------------------------------------------------------------
inline void AWEVector::InvRotateWith(const AWEMatrix& m)
{

	// Using transposed matrix
	float _x = x * m._11 + y * m._12 + z * m._13;
	float _y = x * m._21 + y * m._22 + z * m._23;
	float _z = x * m._31 + y * m._32 + z * m._33;
	x = _x;   y = _y;   z = _z;
}

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Add two vectors together. Note this is faster than '+' due to lack
//		 of additional constructor and return.
//--------------------------------------------------------------------
void AWEVector::operator += (const AWEVector& v)
{
	x += v.x;   y += v.y;   z += v.z;
}

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Add two vectors together. Note this is slower than '+=' due to 
//		 additional constructor and return.
//--------------------------------------------------------------------


AWEVector AWEVector::operator + (const AWEVector& v) const
{
	return AWEVector(x + v.x, y + v.y, z + v.z);
}


void AWEVector::operator -= (const AWEVector& v)
{
	x -= v.x;   y -= v.y;   z -= v.z;
}


AWEVector AWEVector::operator - (const AWEVector& v) const
{
	return AWEVector(x - v.x, y - v.y, z - v.z);
}


void AWEVector::operator *= (float f)
{
	x *= f;   y *= f;   z *= f;
}

void AWEVector::operator /= (float f)
{
	x /= f;   y /= f;   z /= f;
}

AWEVector AWEVector::operator * (float f) const
{
	return AWEVector(x * f, y * f, z * f);
}

AWEVector AWEVector::operator / (float f) const
{
	return AWEVector(x / f, y / f, z / f);
}

void AWEVector::operator += (float f) {
	x += f;   y += f;   z += f;
}

void AWEVector::operator -= (float f)
{
	x -= f;   y -= f;   z -= f;
}

AWEVector AWEVector::operator + (float f) const
{
	return AWEVector(x + f, y + f, z + f);
}

AWEVector AWEVector::operator - (float f) const
{
	return AWEVector(x - f, y - f, z - f);
}


float AWEVector::operator * (const AWEVector& v) const
{
	return (v.x * x + v.y * y + v.z * z);

}

AWEVector AWEVector::operator * (const AWEMatrix& m) const
{
	AWEVector vcResult;


	vcResult.x = x * m._11 + y * m._21 + z * m._31 + m._41;
	vcResult.y = x * m._12 + y * m._22 + z * m._32 + m._42;
	vcResult.z = x * m._13 + y * m._23 + z * m._33 + m._43;
	vcResult.w = x * m._14 + y * m._24 + z * m._34 + m._44;

	vcResult.x = vcResult.x / vcResult.w;
	vcResult.y = vcResult.y / vcResult.w;
	vcResult.z = vcResult.z / vcResult.w;
	vcResult.w = 1.0f;

	return vcResult;
}
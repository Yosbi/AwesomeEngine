//-------------------------------------------------------------------------------
// AWEVector.cpp
// Yosbi Alves
// yosbi@outlook.com
// Copyright (c) 2023
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		AWEVector class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------
#include "AwesomeMath.h"


//--------------------------------------------------------------------
// Name: Constructors
// Desc: 
//--------------------------------------------------------------------
AWEVector::AWEVector(): m_vector(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)) {}
AWEVector::AWEVector(float _x, float _y, float _z): m_vector(DirectX::XMVectorSet(_x, _y, _z, 1.0f)) {}
AWEVector::AWEVector(const AWEVector& other) : m_vector(other.m_vector) {}
AWEVector::AWEVector(const DirectX::XMVECTOR& v) : m_vector(v) {}


//--------------------------------------------------------------------
// Name: Getters
// Desc: 
//--------------------------------------------------------------------
inline float AWEVector::GetX() const { return DirectX::XMVectorGetX(m_vector); }
inline float AWEVector::GetY() const { return DirectX::XMVectorGetY(m_vector); }
inline float AWEVector::GetZ() const { return DirectX::XMVectorGetZ(m_vector); }
inline float AWEVector::GetW() const { return DirectX::XMVectorGetW(m_vector); }


//--------------------------------------------------------------------
// Name: Setters
// Desc: 
//--------------------------------------------------------------------
inline void AWEVector::SetX(float x) { m_vector = DirectX::XMVectorSetX(m_vector, x); }
inline void AWEVector::SetY(float y) { m_vector = DirectX::XMVectorSetY(m_vector, y); }
inline void AWEVector::SetZ(float z) { m_vector = DirectX::XMVectorSetZ(m_vector, z); }
inline void AWEVector::SetW(float w) { m_vector = DirectX::XMVectorSetW(m_vector, w); }



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
	m_vector = DirectX::XMVectorSet(_x, _y, _z, _w);
}

//--------------------------------------------------------------------
// Name: GetLength()
// Desc: returns the length of the vector(magnitude)
//--------------------------------------------------------------------
inline float AWEVector::GetLength()
{
	return DirectX::XMVectorGetX(DirectX::XMVector3Length(m_vector));
}

//--------------------------------------------------------------------
// Name: GetSqrLength()
// Desc: returns the length of the vector(magnitude) without sqrt
//--------------------------------------------------------------------
inline float AWEVector::GetSqrLength() const
{
	return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(m_vector));
}

//--------------------------------------------------------------------
// Name: Negate()
// Desc: returns a vector negated
//--------------------------------------------------------------------
inline void AWEVector::Negate()
{
	m_vector = DirectX::XMVectorNegate(m_vector);
}

//--------------------------------------------------------------------
// Name: AngleWith()
// Desc: returns the angle between two vectors
//--------------------------------------------------------------------
inline float AWEVector::AngleWith(AWEVector& v)
{
	//return (float)acos(((*this) * v) / (this->GetLength() * v.GetLength()));
	DirectX::XMVECTOR thisNormalized = DirectX::XMVector3Normalize(m_vector);
	DirectX::XMVECTOR otherNormalized = DirectX::XMVector3Normalize(v.m_vector);

	return DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenNormals(thisNormalized, otherNormalized));
}

//--------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize the vector
//--------------------------------------------------------------------
inline void AWEVector::Normalize()
{
	m_vector = DirectX::XMVector3Normalize(m_vector);
}


//--------------------------------------------------------------------
// Name: Cross()
// Desc: Build cross prodict of two vectors.
//--------------------------------------------------------------------
inline AWEVector AWEVector::Cross(const AWEVector& other)
{
	return AWEVector(DirectX::XMVector3Cross(m_vector, other.m_vector));
}

//--------------------------------------------------------------------
// Name: RotateWith()
// Desc: Rotate the vector with the given matrix, ignoring translation
//		 information in matrix if they are present
//--------------------------------------------------------------------
inline void AWEVector::RotateWith(const AWEMatrix& m)
{
	m_vector = DirectX::XMVector3TransformCoord(m_vector, m.m_matrix);

	/*// Applying rotational part of matrix only
	float _x = x * m._11 + y * m._21 + z * m._31;
	float _y = x * m._12 + y * m._22 + z * m._32;
	float _z = x * m._13 + y * m._23 + z * m._33;
	x = _x;   y = _y;   z = _z;*/
}

//--------------------------------------------------------------------
// Name: InvRotateWith()
// Desc: Rotate the vector with the inverse rotation part of the given
//		 matrix, ignoring translation of the matrix if present.
//--------------------------------------------------------------------
inline void AWEVector::InvRotateWith(const AWEMatrix& m)
{

	// Extract the 3x3 rotation matrix (ignoring translation)
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationMatrix(m.m_matrix));

	// Calculate the inverse of the rotation matrix
	DirectX::XMMATRIX inverseRotationMatrix = DirectX::XMMatrixInverse(nullptr, rotationMatrix);

	// Apply the inverse rotation to the vector
	m_vector = DirectX::XMVector3TransformCoord(m_vector, inverseRotationMatrix);

	/*// Using transposed matrix
	float _x = x * m._11 + y * m._12 + z * m._13;
	float _y = x * m._21 + y * m._22 + z * m._23;
	float _z = x * m._31 + y * m._32 + z * m._33;
	x = _x;   y = _y;   z = _z;*/
}

//--------------------------------------------------------------------
// Name: operator +=
// Desc: Add two vectors together. Note this is faster than '+' due to lack
//		 of additional constructor and return.
//--------------------------------------------------------------------
void AWEVector::operator += (const AWEVector& v)
{
	m_vector = DirectX::XMVectorAdd(m_vector, v.m_vector);
}

//--------------------------------------------------------------------
// Name: operator +
// Desc: Add two vectors together. Note this is slower than '+=' due to 
//		 additional constructor and return.
//--------------------------------------------------------------------
AWEVector AWEVector::operator + (const AWEVector& v) const
{
	return AWEVector(DirectX::XMVectorAdd(m_vector, v.m_vector));
}

//--------------------------------------------------------------------
// Name: operator -=
// Desc: 
//--------------------------------------------------------------------
void AWEVector::operator -= (const AWEVector& v)
{
	m_vector = DirectX::XMVectorSubtract(m_vector, v.m_vector);
}

//--------------------------------------------------------------------
// Name: operator -
// Desc: 
//--------------------------------------------------------------------
AWEVector AWEVector::operator - (const AWEVector& v) const
{
	return AWEVector(DirectX::XMVectorSubtract(m_vector, v.m_vector));
}

//--------------------------------------------------------------------
// Name: operator *=
// Desc: 
//--------------------------------------------------------------------
void AWEVector::operator *= (float f)
{
	m_vector = DirectX::XMVectorScale(m_vector, f);
}

//--------------------------------------------------------------------
// Name: operator /=
// Desc: 
//--------------------------------------------------------------------
void AWEVector::operator /= (float f)
{
	if (f != 0.0f)
	{
		float invScalar = 1.0f / f;
		m_vector = DirectX::XMVectorScale(m_vector, invScalar);
	}
}

//--------------------------------------------------------------------
// Name: operator *
// Desc: 
//--------------------------------------------------------------------
AWEVector AWEVector::operator * (float f) const
{
	return AWEVector(DirectX::XMVectorScale(m_vector, f));
}

//--------------------------------------------------------------------
// Name: operator /
// Desc: 
//--------------------------------------------------------------------
AWEVector AWEVector::operator / (float f) const
{
	if (f != 0.0f)
	{
		float invScalar = 1.0f / f;
		return AWEVector(DirectX::XMVectorScale(m_vector, invScalar));
	}

	return AWEVector();
}

//--------------------------------------------------------------------
// Name: operator +=
// Desc: 
//--------------------------------------------------------------------
void AWEVector::operator += (float f) {
	m_vector = DirectX::XMVectorAdd(m_vector, DirectX::XMVectorReplicate(f));
}

//--------------------------------------------------------------------
// Name: operator -=
// Desc:
//--------------------------------------------------------------------
void AWEVector::operator -= (float f)
{
	m_vector = DirectX::XMVectorSubtract(m_vector, DirectX::XMVectorReplicate(f));
}

//--------------------------------------------------------------------
// Name: operator +
// Desc:
//--------------------------------------------------------------------
AWEVector AWEVector::operator + (float f) const
{
	return AWEVector(DirectX::XMVectorAdd(m_vector, DirectX::XMVectorReplicate(f)));
}

//--------------------------------------------------------------------
// Name: operator -
// Desc:
//--------------------------------------------------------------------
AWEVector AWEVector::operator - (float f) const
{
	return AWEVector(DirectX::XMVectorSubtract(m_vector, DirectX::XMVectorReplicate(f)));
}

//--------------------------------------------------------------------
// Name: operator ~*
// Desc:
//--------------------------------------------------------------------
float AWEVector::operator * (const AWEVector& v) const
{
	return DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_vector, v.m_vector));


}

//--------------------------------------------------------------------
// Name: operator *
// Desc:
//--------------------------------------------------------------------
AWEVector AWEVector::operator * (const AWEMatrix& m) const
{
	return AWEVector(DirectX::XMVector4Transform(m_vector, m.m_matrix));
}
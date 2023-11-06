//-------------------------------------------------------------------------------
// AWEQuat.cpp
// Yosbi Alves
// yosbi@outlook.com
// Copyright (c) 2023
//-----------------------------------------------------------------------
// Desc: In this file is the definition of the clases, and methods
//		AWEQuat class
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------

#include "AwesomeMath.h"     

//--------------------------------------------------------------------
// Name: Constructors
// Desc: 
//--------------------------------------------------------------------
AWEQuat::AWEQuat(void) : m_vector(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)) {}
AWEQuat::AWEQuat(float _x, float _y, float _z, float _w) : m_vector(DirectX::XMVectorSet(_x, _y, _z, _w)) {}
AWEQuat::AWEQuat(const AWEQuat& other) : m_vector(other.m_vector) {}
AWEQuat::AWEQuat(const DirectX::XMVECTOR& q) : m_vector(q) {}

//--------------------------------------------------------------------
// Name: Getters
// Desc: 
//--------------------------------------------------------------------
float AWEQuat::GetX() const {
    return DirectX::XMVectorGetX(m_vector);
}

float AWEQuat::GetY() const {
    return DirectX::XMVectorGetY(m_vector);
}

float AWEQuat::GetZ() const {
    return DirectX::XMVectorGetZ(m_vector);
}

float AWEQuat::GetW() const {
    return DirectX::XMVectorGetW(m_vector);
}
//--------------------------------------------------------------------
// Name: Setters
// Desc: 
//--------------------------------------------------------------------
void AWEQuat::SetVector(AWEVector& v) {
    m_vector = v.m_vector;
}

void AWEQuat::SetX(float xValue) {
    m_vector = DirectX::XMVectorSetX(m_vector, xValue);
}

void AWEQuat::SetY(float yValue) {
    m_vector = DirectX::XMVectorSetY(m_vector, yValue);
}

void AWEQuat::SetZ(float zValue) {
    m_vector = DirectX::XMVectorSetZ(m_vector, zValue);
}

void AWEQuat::SetW(float wValue) {
    m_vector = DirectX::XMVectorSetW(m_vector, wValue);
}

//--------------------------------------------------------------------
// Name: GetMagnitude()
// Desc: Get the mgnitude of a quaternion
//--------------------------------------------------------------------
float AWEQuat::GetMagnitude(void) {
    return DirectX::XMVectorGetX(DirectX::XMQuaternionLength(m_vector));
}

//--------------------------------------------------------------------
// Name: Normalize()
// Desc: Normalize quaternion
//--------------------------------------------------------------------
void AWEQuat::Normalize(void) {
    m_vector = DirectX::XMQuaternionNormalize(m_vector);
}

//--------------------------------------------------------------------
// Name: Conjugate()
// Desc: 
//--------------------------------------------------------------------
void AWEQuat::Conjugate(AWEQuat q) {
    m_vector = DirectX::XMQuaternionConjugate(q.m_vector);
}

//--------------------------------------------------------------------
// Name: Rotate()
// Desc: 
//--------------------------------------------------------------------
void AWEQuat::Rotate(const AWEQuat& q1, const AWEQuat& q2) {
    m_vector = DirectX::XMQuaternionMultiply(q2.m_vector, q1.m_vector);
}

//--------------------------------------------------------------------
// Name: Rotate()
// Desc: 
//--------------------------------------------------------------------
AWEVector AWEQuat::Rotate(const AWEVector& v) {
    return AWEVector(DirectX::XMVector3Rotate(v.m_vector, m_vector));
}

//--------------------------------------------------------------------
// Name: GetMatrix()
// Desc: Get a rotation matrix out of the quaternion
//--------------------------------------------------------------------
void AWEQuat::GetMatrix(AWEMatrix& pMat) {
    pMat.m_matrix = DirectX::XMMatrixRotationQuaternion(m_vector);
}

//--------------------------------------------------------------------
// Name: MakeFromEuler()
// Desc: Construct quaternion from Euler angles
//--------------------------------------------------------------------
void AWEQuat::MakeFromEuler(float fPitch, float fYaw, float fRoll) {
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(fPitch, fYaw, fRoll);
    m_vector = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
}

//--------------------------------------------------------------------
// Name: GetEulers()
// Desc: Converts quaternion into euler angles
//--------------------------------------------------------------------
void AWEQuat::GetEulers(float& fPitch, float& fYaw, float& fRoll) {
    // Convert the quaternion to a rotation matrix
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vector);

    // Extract Euler angles from the rotation matrix
    fPitch = asinf(-DirectX::XMVectorGetZ(rotationMatrix.r[1]));
    fYaw = atan2f(DirectX::XMVectorGetX(rotationMatrix.r[2]), DirectX::XMVectorGetY(rotationMatrix.r[0]));
    fRoll = atan2f(DirectX::XMVectorGetX(rotationMatrix.r[1]), DirectX::XMVectorGetY(rotationMatrix.r[1]));
}

//--------------------------------------------------------------------
// Name: operator /= 
// Desc: 
//--------------------------------------------------------------------
void AWEQuat::operator /= (float f)
{
    AWEVector v(m_vector);
    v = v / f; 
    m_vector = v.m_vector;
}

//--------------------------------------------------------------------
// Name: operator ~ 
// Desc: 
//--------------------------------------------------------------------
AWEQuat AWEQuat::operator~(void) const 
{ 
    return AWEQuat(-GetX(), -GetY(), -GetZ(), GetW());
}


//--------------------------------------------------------------------
// Name: operator /
// Desc: 
//--------------------------------------------------------------------
AWEQuat AWEQuat::operator / (float f) {
    AWEVector v(m_vector);
    v = v / f;
    return AWEQuat(v.m_vector);
}

//--------------------------------------------------------------------
// Name: operator *=
// Desc: Multiplies a quaternion by a scalar value
//--------------------------------------------------------------------
void AWEQuat::operator *= (float f)
{
    m_vector = DirectX::XMQuaternionMultiply(m_vector, DirectX::XMVectorSet(f, f, f, f));
}

//--------------------------------------------------------------------
// Name: operator *
// Desc: Multiplies a quaternion by a scalar value
//--------------------------------------------------------------------
AWEQuat AWEQuat::operator * (float f) {
    return AWEQuat(DirectX::XMQuaternionMultiply(m_vector, DirectX::XMVectorSet(f, f, f, f)));
}

//--------------------------------------------------------------------
// Name: operator +=
// Desc: Adds two quaternions
//--------------------------------------------------------------------
void AWEQuat::operator += (const AWEQuat& q)
{
    m_vector = DirectX::XMVectorAdd(m_vector, q.m_vector);
}

AWEQuat AWEQuat::operator + (const AWEQuat& q) const {
    return AWEQuat(DirectX::XMVectorAdd(m_vector, q.m_vector));
}


//--------------------------------------------------------------------
// Name: operator *=
// Desc: multiplies two quaternions with *= operator
//--------------------------------------------------------------------
// multiplies two quaternions with *= operator
void AWEQuat::operator *= (const AWEQuat& q) {
    m_vector = DirectX::XMQuaternionMultiply(m_vector, q.m_vector);
}

//--------------------------------------------------------------------
// Name: operator *
// Desc: Multiplies two quaternions with *= operator
// multiplies two quaternions with * operator
//--------------------------------------------------------------------
AWEQuat AWEQuat::operator * (const AWEQuat& q) const {
   return AWEQuat(DirectX::XMQuaternionMultiply(m_vector, q.m_vector));
}

//--------------------------------------------------------------------
// Name: operator *
// Desc: Multiplies a vector with a quaternion
//--------------------------------------------------------------------
AWEQuat AWEQuat::operator * (const AWEVector& v) const {
    return AWEQuat(DirectX::XMVector3Rotate(v.m_vector, m_vector));
}





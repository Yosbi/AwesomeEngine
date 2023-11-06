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
AWEMatrix::AWEMatrix(const AWEMatrix& other) : m_matrix(other.m_matrix) {}
AWEMatrix::AWEMatrix(const DirectX::XMMATRIX& m) : m_matrix(m) {}

//--------------------------------------------------------------------
// Name: GetElement
// Desc: Getter for specific matrix elements
//--------------------------------------------------------------------
float AWEMatrix::GetElement(int row, int col) const {
	if (row >= 0 && row < 4 && col >= 0 && col < 4) {
		return DirectX::XMVectorGetByIndex(m_matrix.r[row], col);
	}
	// Return some default value or handle the error as needed
	return 0.0f;
}

//--------------------------------------------------------------------
// Name: SetElement()
// Desc: Setter for specific matrix elements
//--------------------------------------------------------------------
void AWEMatrix::SetElement(int row, int col, float value) {
	if (row >= 0 && row < 4 && col >= 0 && col < 4) {
		m_matrix.r[row] = DirectX::XMVectorSetByIndex(m_matrix.r[row], value, col);
	}
	// TODO: handle the error if indices are out of bounds
}


//--------------------------------------------------------------------
// Name: AWEMatrix()
// Desc: Transform a matrix in a identity matrix
//--------------------------------------------------------------------
inline void AWEMatrix::Identity(void)
{
	m_matrix = DirectX::XMMatrixIdentity();
}

//--------------------------------------------------------------------
// Name: RotaX()
// Desc: Build rotation matrix around X axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaX(float a) {
	m_matrix *= DirectX::XMMatrixRotationX(a);
}

//--------------------------------------------------------------------
// Name: RotaY()
// Desc: Build rotation matrix around Y axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaY(float a) {
	m_matrix *= DirectX::XMMatrixRotationY(a);
}

//--------------------------------------------------------------------
// Name: RotaZ()
// Desc: Build rotation matrix around Z axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaZ(float a) {
	m_matrix *= DirectX::XMMatrixRotationZ(a);
}

//--------------------------------------------------------------------
// Name: YApplyInverseRota()
// Desc: Apply an inverse rotation arround a vector
//--------------------------------------------------------------------
inline void AWEMatrix::ApplyInverseRota(AWEVector& pvc) {
	// Calculate the inverse rotation matrix
	DirectX::XMMATRIX inverseRotationMatrix = DirectX::XMMatrixInverse(nullptr, m_matrix);

	// Transform the vector with the inverse rotation matrix
	pvc.m_vector = DirectX::XMVector3TransformCoord(pvc.m_vector, inverseRotationMatrix);
}

//--------------------------------------------------------------------
// Name: Rota()
// Desc: Apply a rotation arround x,y,z angles
//--------------------------------------------------------------------
inline void AWEMatrix::Rota(float x, float y, float z)
{
	m_matrix *= DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
}

//--------------------------------------------------------------------
// Name: YRota()
// Desc: Apply a rotation arround the vector
//--------------------------------------------------------------------
inline void AWEMatrix::Rota(const AWEVector& vc) {
	m_matrix *= DirectX::XMMatrixRotationRollPitchYaw(vc.GetX(), vc.GetY(), vc.GetZ());
}

//--------------------------------------------------------------------
// Name: YSetTranslation()
// Desc: Set translation values,  if the boolean b is true, then
//		the matrix is set first to an indentity matrix
//--------------------------------------------------------------------
inline void AWEMatrix::SetTranslation(AWEVector vc, bool EraseContent) {
	if (EraseContent) {
		m_matrix.r[3] = DirectX::XMVectorSet(vc.GetX(), vc.GetY(), vc.GetZ(), 1.0f);
	}
	else {
		m_matrix.r[3] = DirectX::XMVectorAdd(m_matrix.r[3], vc.m_vector);
	}
} 

//--------------------------------------------------------------------
// Name: GetTranslation()
// Desc: Get translation values
//--------------------------------------------------------------------
inline AWEVector AWEMatrix::GetTranslation(void)
{
	return AWEVector(m_matrix.r[3]);
}

//--------------------------------------------------------------------
// Name: RotaArbi()
// Desc: Build rotation matrix around arbitrary axis
//--------------------------------------------------------------------
inline void AWEMatrix::RotaArbi(const AWEVector& _vcAxis, float a) {
	AWEVector vcAxis = _vcAxis;

	// Ensure the axis is normalized
	if (vcAxis.GetSqrLength() != 1.0f)
		vcAxis.Normalize();

	// Create a rotation matrix around the arbitrary axis
	m_matrix = DirectX::XMMatrixRotationAxis(vcAxis.m_vector, a);
}

//--------------------------------------------------------------------
// Name: Translate()
// Desc: Add translation to matrix
//--------------------------------------------------------------------
inline void AWEMatrix::Translate(float dx, float dy, float dz) {
	m_matrix.r[3] = DirectX::XMVectorSet(dx, dy, dz, 1.0f);
}

//--------------------------------------------------------------------
// Name: LookAt()
// Desc: look from given position at given point
//--------------------------------------------------------------------

inline void AWEMatrix::LookAt(AWEVector vcPos, AWEVector vcLookAt, AWEVector vcWorldUp)
{
	m_matrix = DirectX::XMMatrixLookAtLH(vcPos.m_vector, vcLookAt.m_vector, vcWorldUp.m_vector);
}

//--------------------------------------------------------------------
// Name: Operator * ()
// Desc: Multiply two matrices
//-------------------------------------------------------------------- 
AWEMatrix AWEMatrix::operator * (const AWEMatrix& m) const {
	return AWEMatrix(DirectX::XMMatrixMultiply(m_matrix, m.m_matrix));
}

//--------------------------------------------------------------------
// Name: Operator * ()
// Desc: Multiply matrix with a vector
//-------------------------------------------------------------------- 
AWEVector AWEMatrix::operator * (const AWEVector& vc) const {
	return  AWEVector(DirectX::XMVector3Transform(vc.m_vector, m_matrix));
}

//--------------------------------------------------------------------
// Name: TransposeOf()
// Desc: Transpose the matrix
//--------------------------------------------------------------------
inline void AWEMatrix::TransposeOf(const AWEMatrix& m) {
	m_matrix = DirectX::XMMatrixTranspose(m.m_matrix);
}

//--------------------------------------------------------------------
// Name: Scale()
// Desc: Make a scale matrix to the d factors
//--------------------------------------------------------------------
inline void AWEMatrix::Scale(float dx, float dy, float dz)
{
	this->SetElement(1, 1, dx);
	this->SetElement(2, 2, dy);
	this->SetElement(3, 3, dz);
	this->SetElement(4, 4, 1.0f);
}

//--------------------------------------------------------------------
// Name: InverseOf()
// Desc: invert the matrix, use of intel's SSE code is incredibly slow here.
//--------------------------------------------------------------------
inline void AWEMatrix::InverseOf(const AWEMatrix& m) {
	m_matrix = DirectX::XMMatrixInverse(nullptr, m.m_matrix);
}

#include "AweD3D.h"

//-----------------------------------------------------------------------------
// Name: setClippingPlanes
// Desc: Sets the clipping planes near and far
//-----------------------------------------------------------------------------
void AweD3D::setClippingPlanes(float fnear, float ffar)
{
	m_fNear = fnear;
	m_fFar = ffar;
	SetProjMatrix();
}

//-----------------------------------------------------------------------------
// Name: setFoV
// Desc: Sets the Field of view angle
//-----------------------------------------------------------------------------
void AweD3D::setFoV(float FoV)
{
	m_fFoV = FoV;
	SetProjMatrix();
}

//-----------------------------------------------------------------------------
// Name: SetViewMatrix
// Desc: Computes the view matrix from position, point to look at and the 
//		 world up
//-----------------------------------------------------------------------------
void AweD3D::SetViewMatrix(const AWEVector& vcPos, const AWEVector& vcPoint, const AWEVector& vcWorldUp)
{
	const DirectX::XMVECTOR xeyePosition = DirectX::XMVectorSet(vcPos.x, vcPos.y, vcPos.z, vcPos.w);
	const DirectX::XMVECTOR xfocusPoint  = DirectX::XMVectorSet(vcPoint.x, vcPoint.y, vcPoint.z, vcPoint.w);
	const DirectX::XMVECTOR xupDirection = DirectX::XMVectorSet(vcWorldUp.x, vcWorldUp.y, vcWorldUp.z, vcWorldUp.w);

	DirectX::XMMATRIX xMatrixLookTo = DirectX::XMMatrixLookToLH(xeyePosition, xfocusPoint, xupDirection);
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*) &m_ViewMatrix, xMatrixLookTo);
}

//-----------------------------------------------------------------------------
// Name: SetProjMatrix
// Desc: Computes the projection matrix
//-----------------------------------------------------------------------------
void AweD3D::SetProjMatrix()
{
	float aspectRatio = m_nClientWidth / static_cast<float>(m_nClientHeight);
	DirectX::XMMATRIX xMatrixProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fFoV), aspectRatio,  m_fNear, m_fFar);

	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_ProjectionMatrix, xMatrixProj);
}

//-----------------------------------------------------------------------------
// Name: ComputeMVPMatrix
// Desc: Computes model(world)-view-projection matrix and updates the 
//		 AWED3DENGINEVARS register
//-----------------------------------------------------------------------------
void AweD3D::ComputeMVPMatrix()
{
	DirectX::XMMATRIX xWorldMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_WorldMatrix);
	DirectX::XMMATRIX xViewMatrix  = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_ViewMatrix);
	DirectX::XMMATRIX xProjMatrix  = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_ProjectionMatrix);

	DirectX::XMMATRIX xmvpMatrix = XMMatrixMultiply(xWorldMatrix, xViewMatrix);
	xmvpMatrix = XMMatrixMultiply(xmvpMatrix, xProjMatrix);
	xmvpMatrix = XMMatrixTranspose(xmvpMatrix);

	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_EngineVariables.mWVP, xmvpMatrix);
}

//-----------------------------------------------------------------------------
// Name: SetWorldTransformMatrix
// Desc: Sets the world transformation matrix
//-----------------------------------------------------------------------------
void AweD3D::SetWorldTransformMatrix(const AWEMatrix& mWorld)
{
	m_WorldMatrix = mWorld;
}
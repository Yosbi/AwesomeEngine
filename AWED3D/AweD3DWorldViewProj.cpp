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
	const DirectX::XMVECTOR xfocusPoint = DirectX::XMVectorSet(vcPoint.x, vcPoint.y, vcPoint.z, vcPoint.w);
	const DirectX::XMVECTOR xupDirection = DirectX::XMVectorSet(vcWorldUp.x, vcWorldUp.y, vcWorldUp.z, vcWorldUp.w);

	m_ViewMatrix = DirectX::XMMatrixLookToLH(xeyePosition, xfocusPoint, xupDirection);
}

//-----------------------------------------------------------------------------
// Name: SetProjMatrix
// Desc: Computes the projection matrix
//-----------------------------------------------------------------------------
void AweD3D::SetProjMatrix()
{
	float aspectRatio = m_nClientWidth / static_cast<float>(m_nClientHeight);
	m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fFoV), aspectRatio,  m_fNear, m_fFar);
}

//-----------------------------------------------------------------------------
// Name: GetMVPMatrix
// Desc: Computes and returns the model(world)-view-projection matrix
//-----------------------------------------------------------------------------
DirectX::XMMATRIX AweD3D::GetMVPMatrix()
{
	DirectX::XMMATRIX mvpMatrix = XMMatrixMultiply(m_WorldMatrix, m_ViewMatrix);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
	mvpMatrix = XMMatrixTranspose(mvpMatrix);

	return mvpMatrix;
}

//-----------------------------------------------------------------------------
// Name: SetWorldTransformMatrix
// Desc: Sets the world transformation matrix
//-----------------------------------------------------------------------------
void AweD3D::SetWorldTransformMatrix(const AWEMatrix& mWorld)
{
	m_WorldMatrix = DirectX::XMMatrixSet(
		mWorld._11, mWorld._12, mWorld._13, mWorld._14,
		mWorld._21, mWorld._22, mWorld._23, mWorld._24,
		mWorld._31, mWorld._32, mWorld._33, mWorld._34,
		mWorld._41, mWorld._42, mWorld._43, mWorld._44
	);
}
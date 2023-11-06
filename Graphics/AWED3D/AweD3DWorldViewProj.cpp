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
	const DirectX::XMVECTOR xeyePosition = DirectX::XMVectorSet(vcPos.GetX(), vcPos.GetY(), vcPos.GetZ(), vcPos.GetW());
	const DirectX::XMVECTOR xfocusPoint  = DirectX::XMVectorSet(vcPoint.GetX(), vcPoint.GetY(), vcPoint.GetZ(), vcPoint.GetW());
	const DirectX::XMVECTOR xupDirection = DirectX::XMVectorSet(vcWorldUp.GetX(), vcWorldUp.GetY(), vcWorldUp.GetZ(), vcWorldUp.GetW());

	DirectX::XMMATRIX xMatrixLookTo = DirectX::XMMatrixLookToLH(xeyePosition, xfocusPoint, xupDirection);
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*) &m_ViewMatrix, xMatrixLookTo);

	// Updating the pass variables CB
	m_PassVariables.vcEyePosW = vcPos;
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
// Name: UpdateMainPassVariablesCB
// Desc: Computes the main pass variables
//-----------------------------------------------------------------------------
void AweD3D::UpdateMainPassVariablesCB()
{
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_ViewMatrix);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_ProjectionMatrix);
	DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&XMMatrixDeterminant(view), view);
	DirectX::XMMATRIX invProj = DirectX::XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PassVariables.mView, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PassVariables.mInvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PassVariables.mProj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PassVariables.mInvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PassVariables.mViewProj, DirectX::XMMatrixTranspose(viewProj));
	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PassVariables.mInvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	
	m_CbUploadPassVariables->CopyData(0, m_PassVariables);

	


	/*

	DirectX::XMMATRIX xWorldMatrix = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_WorldMatrix);
	DirectX::XMMATRIX xViewMatrix  = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_ViewMatrix);
	DirectX::XMMATRIX xProjMatrix  = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_ProjectionMatrix);

	DirectX::XMMATRIX xmvpMatrix = XMMatrixMultiply(xWorldMatrix, xViewMatrix);
	xmvpMatrix = XMMatrixMultiply(xmvpMatrix, xProjMatrix);
	xmvpMatrix = XMMatrixTranspose(xmvpMatrix);

	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_EngineVariables.mWVP, xmvpMatrix);*/
}

//-----------------------------------------------------------------------------
// Name: UpdatePerObjectVariablesCB
// Desc: Computes the main per object variables
//-----------------------------------------------------------------------------
void AweD3D::UpdatePerObjectVariablesCB()
{
	m_CbUploadPerObjectVariables->CopyData(0, m_PerObjectVariables);
}

//-----------------------------------------------------------------------------
// Name: SetWorldTransformMatrix
// Desc: Sets the world transformation matrix, also computes the inverse 
//		 transpose of the matrix 
//-----------------------------------------------------------------------------
void AweD3D::SetWorldTransformMatrix(const AWEMatrix& mWorld)
{
	m_PerObjectVariables.mWorld = mWorld;
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&m_PerObjectVariables.mWorld);
	XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PerObjectVariables.mWorld, XMMatrixTranspose(world));

	// Calculating the inverse transpose taking advantage of the DirectX's SIMD 
	DirectX::XMMATRIX xWorldInverseTranspose = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&mWorld);
	xWorldInverseTranspose = XMMatrixInverse(0, xWorldInverseTranspose);
	xWorldInverseTranspose = XMMatrixTranspose(xWorldInverseTranspose);

	DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&m_PerObjectVariables.mWorldInverseTranspose, xWorldInverseTranspose);

}
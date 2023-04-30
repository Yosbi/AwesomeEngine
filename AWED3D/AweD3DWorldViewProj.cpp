#include "AweD3D.h"


void AweD3D::setClippingPlanes(float fnear, float ffar)
{
	m_fNear = fnear;
	m_fFar = ffar;
	SetProjMatrix();
}
void AweD3D::setFoV(float FoV)
{
	m_fFoV = FoV;
	SetProjMatrix();
}

void AweD3D::SetViewMatrix(const AWEVector& vcPos, const AWEVector& vcPoint, const AWEVector& vcWorldUp)
{
	const DirectX::XMVECTOR xeyePosition = DirectX::XMVectorSet(vcPos.x, vcPos.y, vcPos.z, vcPos.w);
	const DirectX::XMVECTOR xfocusPoint = DirectX::XMVectorSet(vcPoint.x, vcPoint.y, vcPoint.z, vcPoint.w);
	const DirectX::XMVECTOR xupDirection = DirectX::XMVectorSet(vcWorldUp.x, vcWorldUp.y, vcWorldUp.z, vcWorldUp.w);

	m_ViewMatrix = DirectX::XMMatrixLookToLH(xeyePosition, xfocusPoint, xupDirection);
}

void AweD3D::SetProjMatrix()
{
	float aspectRatio = m_nClientWidth / static_cast<float>(m_nClientHeight);
	m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fFoV), aspectRatio,  m_fNear, m_fFar);
}

DirectX::XMMATRIX AweD3D::GetMVPMatrix(AweMesh* mesh)
{
	DirectX::XMMATRIX mvpMatrix = XMMatrixMultiply(mesh->getModelMatrix(), m_ViewMatrix);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
	mvpMatrix = XMMatrixTranspose(mvpMatrix);

	return mvpMatrix;
}
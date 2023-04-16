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
void AweD3D::SetViewMatrix(const AWEVector& eyePosition, const AWEVector& focusPoint, const AWEVector& upDirection)
{
	const XMVECTOR xeyePosition = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, eyePosition.w);
	const XMVECTOR xfocusPoint = XMVectorSet(focusPoint.x, focusPoint.y, focusPoint.z, focusPoint.w);
	const XMVECTOR xupDirection = XMVectorSet(upDirection.x, upDirection.y, upDirection.z, upDirection.w);
	m_ViewMatrix = XMMatrixLookAtLH(xeyePosition, xfocusPoint, xupDirection);
}
void AweD3D::SetProjMatrix()
{
	float aspectRatio = m_nClientWidth / static_cast<float>(m_nClientHeight);
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFoV), aspectRatio, 0.1f, 100.0f);
}

XMMATRIX AweD3D::GetMVPMatrix(AweMesh* mesh)
{
	XMMATRIX mvpMatrix = XMMatrixMultiply(mesh->getModelMatrix(), m_ViewMatrix);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
	mvpMatrix = XMMatrixTranspose(mvpMatrix);

	return mvpMatrix;
}
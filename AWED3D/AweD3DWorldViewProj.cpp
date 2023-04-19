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
UINT AweD3D::CreateCamera()
{
	return CreateCamera(AWEVector(0, 0, -30), AWEVector(0, 0, 0), AWEVector(0, 1, 0));
}
UINT AweD3D::CreateCamera(AWEVector eyePosition, AWEVector focusPoint, AWEVector upDirection)
{
	m_cameras.push_back(AweCamera(eyePosition, focusPoint, upDirection));
	return m_cameras.size() - 1;
}
void AweD3D::moveCameraPositionDelta(UINT cameraIndex, AWEVector deltaPos)
{
	// TODO: handle exception
	m_cameras.at(cameraIndex).moveDelta(deltaPos);
}
void AweD3D::moveCameraFocusDelta(UINT cameraIndex, AWEVector deltaPos)
{
	m_cameras.at(cameraIndex).moveFocusDelta(deltaPos);
}
void AweD3D::SetViewMatrix(unsigned int cameraIndex)
{
	// TODO: Handle out of bound error
	AweCamera* pCamera = &m_cameras.at(cameraIndex);
	AWEVector eyePosition = pCamera->getEyePosition();
	AWEVector focusPoint = pCamera->getFocus();
	AWEVector upDirection = pCamera->getUpDirection();

	const XMVECTOR xeyePosition = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, eyePosition.w);
	const XMVECTOR xfocusPoint = XMVectorSet(focusPoint.x, focusPoint.y, focusPoint.z, focusPoint.w);
	const XMVECTOR xupDirection = XMVectorSet(upDirection.x, upDirection.y, upDirection.z, upDirection.w);
	m_ViewMatrix = XMMatrixLookAtLH(xeyePosition, xfocusPoint, xupDirection);
}
void AweD3D::SetProjMatrix()
{
	float aspectRatio = m_nClientWidth / static_cast<float>(m_nClientHeight);
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFoV), aspectRatio,  m_fNear, m_fFar);
}

XMMATRIX AweD3D::GetMVPMatrix(AweMesh* mesh)
{
	XMMATRIX mvpMatrix = XMMatrixMultiply(mesh->getModelMatrix(), m_ViewMatrix);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
	mvpMatrix = XMMatrixTranspose(mvpMatrix);

	return mvpMatrix;
}
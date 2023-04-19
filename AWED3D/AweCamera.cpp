#include "AweCamera.h"

AweCamera::AweCamera():
	m_eyeInitialPosition(AWEVector( 0.0f, 0.0f, 0.0f)),
	m_eyePosition(AWEVector(0.0f, 0.0f, 0.0f)),
	m_focusPoint(AWEVector(0.0f, 0.0f, 0.0f)),
	m_upDirection(AWEVector(0.0f, 1.0f, 0.0f))
{}

AweCamera::AweCamera(AWEVector eyePosition, AWEVector focusPoint, AWEVector upDirection)
{
	setEyeInitialPosition(eyePosition);
	setEyePosition(eyePosition);
	setFocus(focusPoint);
	setUpDirection(upDirection);
}

AweCamera::~AweCamera()
{
	/* Nothing to do yet*/
}

void AweCamera::setEyeInitialPosition(AWEVector position)
{
	m_eyeInitialPosition = position;
}

void AweCamera::setEyePosition(AWEVector position)
{
	m_eyePosition = position;
}

void AweCamera::setFocus(AWEVector point)
{
	m_focusPoint = point;
}

void AweCamera::setUpDirection(AWEVector upVector)
{
	m_upDirection = upVector;
}

void AweCamera::updateCamera(AWEVector position, AWEVector point)
{
	setEyePosition(position);
	setFocus(point);
}

AWEVector AweCamera::getEyePosition() const
{
	return m_eyePosition;
}

AWEVector AweCamera::getFocus() const
{
	return m_focusPoint;
}

AWEVector AweCamera::getUpDirection() const
{
	return m_upDirection;
}

void AweCamera::moveDelta(AWEVector deltaPos)
{
	m_eyePosition.x += deltaPos.x;
	m_eyePosition.y += deltaPos.y;
	m_eyePosition.z += deltaPos.z;
}

void AweCamera::moveFocusDelta(AWEVector deltaPos)
{
	m_focusPoint.x += deltaPos.x;
	m_focusPoint.y += deltaPos.y;
	m_focusPoint.z += deltaPos.z;
}

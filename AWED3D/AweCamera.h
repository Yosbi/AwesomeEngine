//-----------------------------------------------------------------------
// AweD3D.h
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 17/04/2023
//-----------------------------------------------------------------------
// Desc: This file contains all related functions, methods an data structs
//		to manipulate, initialize, and move a camera
#ifndef AWE_CAMERA
#define AWE_CAMERA
#pragma once

#include "../AwesomeMath/AwesomeMath.h"
#include <DirectXMath.h>

class AweCamera {
public:
	AweCamera();
	AweCamera(AWEVector eyePosition, AWEVector focusPoint, AWEVector upDirection);
	~AweCamera();

	void setEyeInitialPosition(AWEVector position);
	void setEyePosition(AWEVector position);
	void setFocus(AWEVector point);
	void setUpDirection(AWEVector upVector);
	void updateCamera(AWEVector position, AWEVector point);

	AWEVector getEyePosition() const;
	AWEVector getFocus() const;
	AWEVector getUpDirection() const;

	void moveDelta(AWEVector deltaPos);
	void moveFocusDelta(AWEVector deltaPos);
	

private:
	AWEVector m_eyeInitialPosition;
	AWEVector m_eyePosition;
	AWEVector m_focusPoint;
	AWEVector m_upDirection;
};

#endif // !AWE_CAMERA


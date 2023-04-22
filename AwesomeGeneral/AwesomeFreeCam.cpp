//-----------------------------------------------------------------------
// AwesomeFreeCam.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
#include "AwesomeGeneral.h"

//--------------------------------------------------------------------
// Name: Constructor
// Desc: Just initialize some values.
//--------------------------------------------------------------------
AwesomeFreeCam::AwesomeFreeCam(void) {
	AwesomeBaseCam::Init();
} 

//--------------------------------------------------------------------
// Name: Destructor
// Desc:
//--------------------------------------------------------------------
AwesomeFreeCam::~AwesomeFreeCam(void) {

} 

//--------------------------------------------------------------------
// Name: SetRotation
// Desc: Set rotation values and recalculate local vectors.
//--------------------------------------------------------------------
void AwesomeFreeCam::SetRotation(float rx, float ry, float rz) {
	m_fRotX = rx;
	m_fRotY = ry;
	m_fRotZ = rz;

	RecalcAxes();
} 

//--------------------------------------------------------------------
// Name: AddRotationSpeed
// Desc: Add speed to the rotation around the axes
//--------------------------------------------------------------------
void AwesomeFreeCam::AddRotationSpeed(float sx, float sy, float sz) {
	m_fPitchSpd += sx;
	m_fYawSpd += sy;
	m_fRollSpd += sz;
}

//--------------------------------------------------------------------
// Name: SetRotationSpeed
// Desc: Set the rotation speed around the axes to this value
//--------------------------------------------------------------------
void AwesomeFreeCam::SetRotationSpeed(float sx, float sy, float sz) {
	m_fPitchSpd = sx;
	m_fYawSpd = sy;
	m_fRollSpd = sz;
} 

//--------------------------------------------------------------------
// Name: Update
// Desc: Calculate movement based on behavior model and attribute values.
//		Input parameter is elapsed time since last frame.
//--------------------------------------------------------------------
void AwesomeFreeCam::Update(float fET) {
	// add rotation speed
	m_fRotX = (m_fPitchSpd * fET);
	m_fRotY = (m_fYawSpd * fET);
	m_fRotZ = (m_fRollSpd * fET);

	// calculate velocity vector
	m_vcV = m_vcDir * m_fThrust * fET;

	// move position according to velocity vector
	m_vcPos += m_vcV;

	// recalculate axes
	RecalcAxes();
}
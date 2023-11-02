//-----------------------------------------------------------------------
// AwesomeBaseCam.cpp
// Yosbi Alves Saenz
// yosbi@outlook.com
// Created on 22/04/2023
//-----------------------------------------------------------------------
#include "AwesomeGeneral.h"

//--------------------------------------------------------------------
// Name: Constructor
// Desc: Just initialize some values.
//--------------------------------------------------------------------
AwesomeBaseCam::AwesomeBaseCam(void) {
	Init();
}

//--------------------------------------------------------------------
// Name: Destructor
// Desc: 
//--------------------------------------------------------------------
AwesomeBaseCam::~AwesomeBaseCam(void) {
} 

//--------------------------------------------------------------------
// Name: Init()
// Desc: Init values
//--------------------------------------------------------------------
void AwesomeBaseCam::Init(void) {
	m_vcPos.Set(0.0f, 0.0f, 0.0f);
	m_vcRight.Set(1.0f, 0.0f, 0.0f);
	m_vcUp.Set(0.0f, 1.0f, 0.0f);
	m_vcDir.Set(0.0f, 0.0f, 1.0f);
	m_vcV.Set(0.0f, 0.0f, 0.0f);
	m_fRotX = m_fRotY = m_fRotZ = m_fThrust = 0.0f;
	m_fRollSpd = m_fPitchSpd = m_fYawSpd = 0.0f;
	m_Quat.x = m_Quat.y = m_Quat.z = 0.0f;
	m_Quat.w = 1.0f;
}

//--------------------------------------------------------------------
// Name: RecalcAxes()
// Desc: Recalculate the orientation of the local axes according to the
// current rotation values around the axes.
//--------------------------------------------------------------------
void AwesomeBaseCam::RecalcAxes(void) {
	AWEQuat    qFrame;
	AWEMatrix  mat;
	
	//OutputDebugStringA((std::to_string(m_fRotX) + "\n").c_str());

	// keep in range of 360 degree
	if (m_fRotX > AWE2PI)
		m_fRotX -= AWE2PI;
	else if (m_fRotX < -AWE2PI)
		m_fRotX += AWE2PI;

	if (m_fRotY > AWE2PI)
		m_fRotY -= AWE2PI;
	else if (m_fRotY < -AWE2PI)
		m_fRotY += AWE2PI;

	if (m_fRotZ > AWE2PI)
		m_fRotZ -= AWE2PI;
	else if (m_fRotZ < -AWE2PI)
		m_fRotZ += AWE2PI;

	// build new quaternion for this rotation
	qFrame.MakeFromEuler(m_fRotX, m_fRotY, m_fRotZ);

	// add to existing rotation
	m_Quat *= qFrame;

	// extract the local axes
	m_Quat.GetMatrix(&mat);

	m_vcRight.x = mat._11;
	m_vcRight.y = mat._21;
	m_vcRight.z = mat._31;

	m_vcUp.x = mat._12;
	m_vcUp.y = mat._22;
	m_vcUp.z = mat._32;

	m_vcDir.x = mat._13;
	m_vcDir.y = mat._23;
	m_vcDir.z = mat._33;
} 